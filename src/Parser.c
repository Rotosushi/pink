/*
  the grammar so far

  term := type
        | affix
        | '(' term ')'
       //  | affix ';' term

  affix := primary
         | primary operator primary
         | primary "<-" affix

  primary := primitive
           | primitive primitive

  primitive := atom
             | unop atom

  atom := identifier
        // should we allow binding types?
        // if yes: why not also allow type susbtitution?
        // aka: procedures abstracting over type expressions?
        // TL;DR: identifier := affix OR identifier := term ???
        // currently it is:
        | identifier := affix
        | nil
        | [0-9]+
        | true
        | false
        | \ identifier (: term)? => affix
        | 'if' affix 'then' affix 'else' affix
        | 'while' affix 'do' affix
        | '(' affix ')'

  type := "Nil"
        | "Int"
        | "Bool"
        | "Poly"
        | "Ref" type
        | type -> type
        | '(' type ')'

  identifier := [a-zA-Z_][a-zA-Z0-9_]*
  integer    := [0-9]+

  binop := operator
  unop  := operator

  operator := [*+-/%<>:=&@!~|$^]+


  the kernel so far includes
    integer operators + - unop(-) * / %
    type    operator  ->

*/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "Utilities.h"
#include "Token.h"
#include "SymbolTable.h"
#include "StringInterner.h"
#include "Ast.h"
#include "Judgement.h"
#include "Judgement.h"
#include "Type.h"
#include "ScalarType.h"
#include "ProcType.h"
#include "Application.h"
#include "Variable.h"
#include "Assignment.h"
#include "Bind.h"
#include "Conditional.h"
#include "Iteration.h"
#include "Binop.h"
#include "Unop.h"
#include "Object.h"
#include "Nil.h"
#include "Integer.h"
#include "Boolean.h"
#include "Parser.h"

struct Parser* CreateParser(SymbolTable* symbols, ScopeSet scope, StringInterner* Iids, StringInterner* Iops, TypeInterner* Itypes, BinopPrecedenceTable* prec_table, BinopTable* bs, UnopTable* us)
{
  Parser* result    = (Parser*)malloc(sizeof(Parser));
  result->lexer     = CreateLexer();
  result->markstack = NULL;
  result->tokbuf    = NULL;
  result->txtbuf    = NULL;
  result->locbuf    = NULL;
  result->idx       = 0;
  result->mkstksz   = 0;
  result->bufsz     = 0;
  result->symbols   = symbols;
  result->scope     = scope;
  result->interned_ids     = Iids;
  result->interned_ops     = Iops;
  result->interned_types   = Itypes;
  result->precedence_table = prec_table;
  result->binops           = bs;
  result->unops            = us;
  return result;
}

void DestroyParser(Parser* p)
{
  if (p->lexer)
  {
      free(p->lexer);
      p->lexer = NULL;
  }

  if (p->markstack)
  {
      free(p->markstack);
      p->markstack = NULL;
  }

  if (p->tokbuf)
  {
    free(p->tokbuf);
    p->tokbuf = NULL;
  }

  if (p->locbuf)
  {
    free(p->locbuf);
    p->locbuf = NULL;
  }

  if (p->txtbuf)
  {
    for(int i = 0; i < p->bufsz; i++)
    {
      free(p->txtbuf[i]);
    }
    free(p->txtbuf);
    p->txtbuf = NULL;
  }

  p->idx     = 0;
  p->mkstksz = 0;
  p->bufsz   = 0;
  free(p);
}

void ResetParser(Parser* p)
{

  if (p->markstack != NULL)
  {
      free(p->markstack);
      p->markstack = NULL;
  }

  if (p->tokbuf != NULL)
  {
    free(p->tokbuf);
    p->tokbuf = NULL;
  }

  if (p->locbuf != NULL)
  {
    free(p->locbuf);
    p->locbuf = NULL;
  }

  if (p->txtbuf != NULL)
  {
    for(int i = 0; i < p->bufsz; i++)
    {
      free(p->txtbuf[i]);
    }
    free(p->txtbuf);
    p->txtbuf = NULL;
  }

  p->idx     = 0;
  p->mkstksz = 0;
  p->bufsz   = 0;
}

// the simpler you want a procedure to feel,
// the more descisions you should delegate to
// it's caller. (for the writer of the procedure at least.)
// the actual caller might feel overwhelmed given too
// much to think about at every call site,
// especially if there is usually one or two
// 'right' values for the extra parameters,
// however, the 'directionality' of the flow
// of information is almost always good to
// delegate to the caller. such as where to
// write the output data. this can make a procedure
// feel like it does more work.
void DumpTokens(Parser* p, FILE* out)
{
  for (int i = 0; i < p->bufsz; i++)
  {
    fprintf(out, "[%s]", ToStringToken(p->tokbuf[i]));
  }
  fprintf(out, "\n");
}

// whereas this procedure is encapsulating
// a single action, composed of a few actions
// done in sequence for sure, however,
// there is no need for the called to provide anything,
// because the action itself is singular.
int mark(Parser* p)
{
  /*
  push the current index onto the mark 'stack'

  ms = [idx0, idx1, ..., idxn]

  ~> ms = [idx0, idx1, ..., idxn, idxn+1]
  */

  // realloc acts like malloc when given a NULL ptr.
  // realloc acts like free when given a size of zero.
  // this is essentially the same strategy as my hash
  // tables of pointers, (interned strings, and Ast nodes)
  p->mkstksz += 1;
  p->markstack = (int*)realloc(p->markstack, sizeof(int) * p->mkstksz);

  p->markstack[p->mkstksz - 1] = p->idx;
  return p->idx;
}

void release(Parser* p)
{
  /*
      pop the top mark off the mark 'stack'

      ms = [idx0, idx1, .., idxn-1, idxn]

      ~> ms = [idx0, idx1, .., idxn-1]
  */

  // this check cannot be removed, even though
  // realloc wouldn't complain, the initial read
  // from a NULL ptr will in fact seg fault.
  if (p->markstack == NULL)
    return;
  else
  {
    int mark = p->markstack[p->mkstksz - 1];
    p->mkstksz -= 1;
    p->markstack = (int*)realloc(p->markstack, sizeof(int) * p->mkstksz);
    p->idx = mark;
  }
}

bool speculating(Parser* p)
{
  return p->mkstksz > 0;
}

void filltok(Parser* p, int num)
{
  // add (num) more tokens to the buffers
  if (p->idx + num > p->bufsz)
  {
    int n = (p->idx + 1) - p->bufsz;

    // notice: this is a strategy that could be employed in a
    //         doubling-in-size procedure for our hash tables.
    p->tokbuf = (Token*)realloc(p->tokbuf, sizeof(Token) * (p->bufsz + n));
    p->txtbuf = (char**)realloc(p->txtbuf, sizeof(char*) * (p->bufsz + n));
    p->locbuf = (Location*)realloc(p->locbuf, sizeof(Location) * (p->bufsz + n));

    for (int j = 0; j < n; j++)
    {
      Token t = T_ERR;

      // this call advances the internal state of the lexer,
      // such that we can no longer observe previous tokens in
      // the string. (since i wrote it, i can also say that the
      // location we were previously at is also overwritten.
      // which is why we are buffering all of the
      // tokens, locations, and substrings that we read in.
      // this allows us to, at some point in the future
      // backtrack and parse arbitrarily complex future
      // langauge extensions. removing this feature would
      // actually not affect the current langauge, as
      // it is LL(1) parsable. I didn't intend for the
      // grammar to shake out to LL(1), but it seemed natural
      // when fitting call expressions and binary expressions
      // together in the way I have. (by making disjoint the
      // sets of binop-expression predictor tokens, and
      // call-expression predictor tokens.)
      t = yyLex(p->lexer);

      p->tokbuf[p->bufsz + j] = t;
      p->txtbuf[p->bufsz + j] = yyText(p->lexer);
      p->locbuf[p->bufsz + j] = *(yyLloc(p->lexer));
    }

    p->bufsz += n;
  }
}

void nextok(Parser* p)
{
  p->idx += 1;

  if (p->idx == p->bufsz && !speculating(p))
    ResetParser(p);

  filltok(p, 1);
}

Token curtok(Parser* p)
{
  if (p->tokbuf == NULL)
    return T_ERR;

  return p->tokbuf[p->idx];
}

char* curtxt(Parser* p)
{
  if (p->txtbuf == NULL)
    return NULL;

  return p->txtbuf[p->idx];
}

Location curloc(Parser* p)
{
  Location loc;
  if (p->locbuf == NULL)
    return loc;

  return p->locbuf[p->idx];
}

bool is_unop(Parser* p, char* uop)
{
  InternedString Iuop = InternString(p->interned_ops, uop);

  if (Iuop == NULL)
    return false;

  UnopEliminatorList* possible = FindUnop(p->unops, Iuop);

  if (possible != NULL)
    return true;
  else
    return false;
}

bool is_binop(Parser* p, char* bop)
{
  InternedString Ibop = InternString(p->interned_ops, bop);

  if (Ibop == NULL)
    return false;

  BinopEliminatorList* possible = FindBinop(p->binops, Ibop);

  if (possible != NULL)
    return true;
  else
    return false;
}

bool is_primary(Token t)
{
  switch (t)
  {
    case T_INTEGER:  case T_TRUE: case T_FALSE:
    case T_NIL:   case T_IDENTIFIER: case T_IF:
    case T_WHILE: case T_LPAREN: case T_BSLASH:
    case T_OPERATOR: case T_TYPE_INT:
    case T_TYPE_NIL: case T_TYPE_BOOL:
      return true;
    default:
      return false;
  }
}

bool is_ender(Token t)
{
  switch (t)
  {
    case T_RPAREN: case T_END: case T_EOF:
    case T_EQRARROW: case T_THEN:
    case T_ELSE: case T_IF: case T_WHILE:
    case T_DO:
      return true;
    default:
      return false;
  }
}

Judgement ParseTerm(Parser* p);
Judgement ParseAffix(Parser* p);
Judgement ParseApplication(Parser* p);
Judgement ParsePrimary(Parser* p);
Judgement ParseConditional(Parser* p);
Judgement ParseIteration(Parser* p);
Judgement ParseLambda(Parser* p);
Judgement ParseUnop(Parser* p);
Judgement ParseInfix(Parser* p, Judgement lhs, int min_prec);

bool is_type_literal(Ast* ast)
{
  bool result = false;
  if (ast->kind == A_OBJ)
    if (ast->obj.kind == O_TYPE)
      result = true;
  return result;
}

/*
affix := application
       | application operator application
       | application "<-" affix
       // | application "." affix
*/
Judgement ParseAffix(Parser* p)
{
  Location  lhsloc = curloc(p);
  Judgement lhsjdgmt;

  if (is_primary(curtok(p)))
  {
    lhsjdgmt = ParseApplication(p);

    if (lhsjdgmt.success == true)
    {
      // parse statements which are composed
      // of two terms separated by a symbol.

      if (curtok(p) == T_OPERATOR && is_binop(p, curtxt(p)))
      {
        lhsjdgmt = ParseInfix(p, lhsjdgmt, 0);
      }
      else if (curtok(p) == T_LARROW)
      {
        nextok(p); // eat '<-'
        // this essentially means that '<-'
        // is a very high precedence binary
        // operator.
        Judgement rhsjdgmt = ParseAffix(p);

        if (rhsjdgmt.success == true)
        {
          Location    rhsloc     = curloc(p);
          Location    assignloc;
          assignloc.first_line   = lhsloc.first_line;
          assignloc.first_column = lhsloc.first_column;
          assignloc.last_line    = rhsloc.first_line;
          assignloc.last_column  = rhsloc.first_column;
          lhsjdgmt.success       = true;
          lhsjdgmt.term          = CreateAstAssignment(lhsjdgmt.term, rhsjdgmt.term, &assignloc);
        }
        else
          return rhsjdgmt; // leaks lhsjdgmt.term
      }
      else if (curtok(p) == T_RARROW)
      {
        nextok(p); // eat '->'

        Judgement rhsjdgmt = ParseAffix(p);

        if (rhsjdgmt.success == true)
        {
          Location    rhsloc     = curloc(p);
          Location    typeloc;
          typeloc.first_line   = lhsloc.first_line;
          typeloc.first_column = lhsloc.first_column;
          typeloc.last_line    = rhsloc.first_line;
          typeloc.last_column  = rhsloc.first_column;

          if (is_type_literal(lhsjdgmt.term) && is_type_literal(rhsjdgmt.term))
          {

            lhsjdgmt.success = true;
            lhsjdgmt.term    = GetProcedureType(p->interned_types, lhsjdgmt.term, rhsjdgmt.term, &typeloc);
          }
          else
          {
            char* c0 = "cannot create a type from non type literals [";
            char* c1 = "[";
            char* c2 = "]";
            char* l  = ToStringAst(lhsjdgmt.term);
            char* r  = ToStringAst(rhsjdgmt.term);
            char* t0 = appendstr(c0, l);
            char* t1 = appendstr(t0, c2);
            char* t2 = appendstr(t1, c1);
            char* t3 = appendstr(t2, r);
            lhsjdgmt.success = false;
            lhsjdgmt.error.loc = typeloc;
            lhsjdgmt.error.dsc = appendstr(t3, c2);
            free(l);
            free(r);
            free(t0);
            free(t1);
            free(t2);
            free(t3);
          }
        }
        else
          return rhsjdgmt; // leaks lhsjdgmt.term
      }
    }
  }
  else
  {
    char* c0 = "expected primary token; instead saw token:[";
    char* c1 = "] text:[";
    char* c2 = "]";
    char* t  = ToStringToken(curtok(p));
    char* t0 = appendstr(c0, t);
    char* t1 = appendstr(t0, c1);
    char* t2 = appendstr(t1, curtxt(p));
    lhsjdgmt.success = false;
    lhsjdgmt.error.loc = curloc(p);
    lhsjdgmt.error.dsc = appendstr(t2, c2);
    free(t);
    free(t0);
    free(t1);
    free(t2);
  }
  return lhsjdgmt;
}


/*

      should the next token be an operator, then
      we need to distinguish between which kind of
      operator it is, and where we could see it.
      with unary operators, we want them to bind
      tightly to the immediate next term, but
      we do not want them to parse a full term
      afterwards (as this leads to unintuitive
      evaluation trees.)

      from the beginning we wanted expressions like
      a b c - f g h
      to parse as
      (a b c) - (f g h)

      now i know this is necessary
      to avoid the reverse case of parsing
      [3 - 4] as [3 (-4)]
      and yeah, you are reading that correctly,
      it's a call expression...
      and hence, does not allow the programmer
      to even input an expression representing
      [3 - 4]
      all this means we cannot parse
      a -b c -d
      as
      a (-b) c (-d)
      we must parse it as
      (a - b) (c - d)
      so to apply unary operations within
      a call expression, one -must- wrap
      the operation within parenthesis.


      as a direct result of the above
      discussion we should recommend that
      unary operators avoid symbolically
      intersecting with binary operators.
      if they do, be aware the grammar rules place
      prefrence on the binary form of the operator.
      this means that if the operator is both a unop
      and a binop, the expression
      a op b will always be the operation taking
      arguments a and b,
      and never the application of a taking as argument
      the unop applied to b.

      TLDR:
      [a - b]
      is never
      [a (-b)]
      it is always
      [a - b]
      this is also valid
      [-a - -b]
      this means that when the programmer
      wants to apply an operation directly
      to the argument to a procedure, they
      -must- wrap that operation in parens
      in order for the entire expression
      to parse as a call expression, otherwise
      we switch to parsing a binary operation,
      however, this is only if the operator
       itself is a binop and a unop. should the
       operator only be defined as a unop then
       the grammar will parse it as an application.
    */
/*
application := primary
             | primary application
*/
Judgement ParseApplication(Parser* p)
{
  Location       lhsloc   = curloc(p);
  Judgement lhsjdgmt = ParsePrimary(p);

  if (lhsjdgmt.success == true)
  {
    // this first check makes total sense,
    // either we see more application expression
    // or we see an operator, but
    // why not check for the positive?
    // i.e. is_unop()?
    // because we want to give precedence to every
    // binop over any unop. to prevent unintuitive
    // trees.
    while ((curtok(p) != T_OPERATOR && is_primary(curtok(p)))
        || (curtok(p) == T_OPERATOR && !is_binop(p, curtxt(p))))
    {
      Judgement rhsjdgmt = ParsePrimary(p);

      if (rhsjdgmt.success == true)
      {
        Location       rhsloc    = curloc(p);
        Location       apploc;
        apploc.first_line   = lhsloc.first_line;
        apploc.first_column = lhsloc.first_column;
        apploc.last_line    = rhsloc.last_line;
        apploc.last_column  = rhsloc.last_column;

        // we want to count the number of arguments
        // held within the call tree such that at
        // each call node, you know how many call nodes
        // are below you. then as we apply the procedure,
        // if it stores that this particular application
        // is curryed, then we should be able to build
        // some logic that can construct a partial application
        // term holding a tuple of values which are the already
        // evaluated arguments, and a pointer to the procedure
        // body taking those arguments. maybe both terms need a
        // count, so that we can construct the partial with
        // knowledge of how many more arguments are needed
        // to complete the application. within the interpreter
        // we need to do nothing to get partial application semantics.
        // precisely because we are using tree style evaluation.
        lhsjdgmt.success         = true;
        lhsjdgmt.term            = CreateAstApplication(lhsjdgmt.term, rhsjdgmt.term, &apploc);
      }
      else
        return rhsjdgmt; // leaks lhsjdgmt.term
    }
  }
  return lhsjdgmt;
}


/*
primary := identifier
      | identifier := affix
      | nil
      | [0-9]+
      | true
      | false
      | \ identifier (: type)? => affix
      | 'if' affix 'then' affix 'else' affix
      | 'while' affix 'do' affix
      | operator atom
      | '(' affix ')'
*/
Judgement ParsePrimary(Parser* p)
{
  Location       lhsloc = curloc(p);
  Judgement lhsjdgmt;

  switch(curtok(p))
  {
    case T_IDENTIFIER:
    {
      // the StringInterner itself disconnects the string we pass in
      // from the buffer of strings we are holding in the parser.
      // by making a copy of each string we pass in to hold for itself
      InternedString id = InternString(p->interned_ids, curtxt(p));
      nextok(p); // eat T_IDENTIFIER

      // I can tell the future!
      // T_COLONEQ is not long for this world!
      // we do want to allow an optional type
      // annotation between the Colon and the
      // Eq here, for the astute programmer :)
      // so we must modify the parser, and thus
      // the lexer eventually to recognize the
      // two symbols separately, and code within
      // the getype for binding which double
      // checks the correctness.
      // it's exactly an extra constraint the
      // programmer can add to the expression.
      if (curtok(p) == T_COLONEQ)
      {
        nextok(p); // eat T_COLONEQ
        lhsjdgmt = ParseAffix(p);

        if (lhsjdgmt.success == true)
        {
          Location  rhsloc    = curloc(p);
          Location  bndloc;
          bndloc.first_line   = lhsloc.first_line;
          bndloc.first_column = lhsloc.first_column;
          bndloc.last_line    = rhsloc.first_line;
          bndloc.last_column  = rhsloc.first_column;

          lhsjdgmt.success    = true;
          lhsjdgmt.term       = CreateAstBind(id, lhsjdgmt.term, &bndloc);
        }
        else
          return lhsjdgmt;
      }
      else
      {
        lhsjdgmt.success   = true;
        lhsjdgmt.term      = CreateAstVariable(id, env->scope, &lhsloc);
      }
      break;
    }
    case T_NIL:
    {
      nextok(p); // eat "nil"
      lhsjdgmt.success = true;
      lhsjdgmt.term    = CreateAstNil(&lhsloc);
      break;
    }
    case T_TYPE_NIL:
    {
      nextok(p); // eat 'Nil'
      lhsjdgmt.success = true;
      lhsjdgmt.term    = GetNilType(p->interned_types, &lhsloc);
      break;
    }
    case T_INTEGER:
    {
      lhsjdgmt.success = true;
      lhsjdgmt.term    = CreateAstInteger(atoi(curtxt(p)), &lhsloc);
      nextok(p); // eat "integer" -after- converting the text
      break;
    }
    case T_TYPE_INT:
    {
      nextok(p); // eat 'Int'
      lhsjdgmt.success = true;
      lhsjdgmt.term    = GetIntegerType(p->interned_types, &lhsloc);
      break;
    }
    case T_TRUE:
    {
      nextok(p); // eat "true"
      lhsjdgmt.success = true;
      lhsjdgmt.term    = CreateAstBoolean(true, &lhsloc);
      break;
    }
    case T_FALSE:
    {
      nextok(p); // eat "false"
      lhsjdgmt.success = true;
      lhsjdgmt.term    = CreateAstBoolean(false, &lhsloc);
      break;
    }
    case T_TYPE_BOOL:
    {
      nextok(p); // eat 'Bool'
      lhsjdgmt.success = true;
      lhsjdgmt.term    = GetBooleanType(p->interned_types, &lhsloc);
      break;
    }
    case T_BSLASH:
    {
      lhsjdgmt = ParseLambda(p);
      break;
    }
    case T_IF:
    {
      lhsjdgmt = ParseConditional(p);
      break;
    }
    case T_WHILE:
    {
      lhsjdgmt = ParseIteration(p);
      break;
    }
    case T_LPAREN:
    {
      nextok(p); // eat '('

      if (is_primary(curtok(p)))
      {
        lhsjdgmt = ParseAffix(p);
      }
      else
      {
        char* c0 = "unexpected token:[";
        char* c1 = "], text:[";
        char* c2 = "] following '(', expected primary term.";
        char* t  = ToStringToken(curtok(p));
        char* t0 = appendstr(c0, t);
        char* t1 = appendstr(t0, c1);
        char* t2 = appendstr(t1, curtxt(p));
        lhsjdgmt.success   = false;
        lhsjdgmt.error.loc = curloc(p);
        lhsjdgmt.error.dsc = appendstr(t2, c2);
        free(t);
        free(t0);
        free(t1);
        free(t2);
      }

      if ((lhsjdgmt.success == true) && (curtok(p) == T_RPAREN))
      {
          nextok(p); // eat ')'
      }
      else if ((lhsjdgmt.success == true) && (curtok(p) != T_RPAREN))
      {
        char* c0 = "unexpected missing ')'; instead saw token:[";
        char* c1 = "], text:[";
        char* c2 = "]";
        char* t  = ToStringToken(curtok(p));
        char* t0 = appendstr(c0, t);
        char* t1 = appendstr(t0, c1);
        char* t2 = appendstr(t1, curtxt(p));
        DestroyAst(lhsjdgmt.term);
        lhsjdgmt.success   = false;
        lhsjdgmt.error.dsc = appendstr(t2, c2);
        lhsjdgmt.error.loc = curloc(p);
        free(t);
        free(t0);
        free(t1);
        free(t2);
      }
      break;
    }

    case T_OPERATOR:
    {
      lhsjdgmt = ParseUnop(p);
      break;
    }

    default:
    {
      char* c0 = "unknown Atom token:[";
      char* c1 = "], text:[";
      char* c2 = "] found";
      char* t  = ToStringToken(curtok(p));
      char* t0 = appendstr(c0, t);
      char* t1 = appendstr(t0, c1);
      char* t2 = appendstr(t1, curtxt(p));
      lhsjdgmt.success   = false;
      lhsjdgmt.error.dsc = appendstr(t2, c2);
      lhsjdgmt.error.loc = curloc(p);
      free(t);
      free(t0);
      free(t1);
      free(t2);
      break;
    }
  }
  return lhsjdgmt;
}

/*
  I think, regarding the dangling else
  problem, i like the solution of distinguishing
  the single alternative conditional and the
  double alternative conditional syntacticly.
  it is simple to implement and to understand.
  plus, i'm not hamstrung by a nailed down syntax
  so i can make changes like this.

  if t1 then t2 else t3
  and:
  if t1 do t2

*/
Judgement ParseConditional(Parser* p)
{
  Location       lhsloc = curloc(p);
  Judgement result;
  if (curtok(p) == T_IF)
  {
    nextok(p); // eat 'if'

    Judgement condjdgmt = ParseAffix(p);

    if (condjdgmt.success == true)
    {
      if (curtok(p) == T_THEN)
      {
        nextok(p); // eat 'then'

        Judgement thenjdgmt = ParseAffix(p);

        if (thenjdgmt.success == true)
        {
          if (curtok(p) == T_ELSE)
          {
            nextok(p); // eat 'else'

            Judgement elsejdgmt = ParseAffix(p);

            if (elsejdgmt.success == true)
            {
              Location  rhsloc    = curloc(p);
              Location  cndloc;
              cndloc.first_line   = lhsloc.first_line;
              cndloc.first_column = lhsloc.first_column;
              cndloc.last_line    = rhsloc.first_line;
              cndloc.last_column  = rhsloc.first_column;

              result.success = true;
              result.term = CreateAstConditional(condjdgmt.term, thenjdgmt.term, elsejdgmt.term, &cndloc);
            }
            else
              result = elsejdgmt;
          }
          else
          {
            char* c0 = "unexpected missing 'else'; instead saw token:[";
            char* c1 = "], text:[";
            char* c2 = "]";
            char* t  = ToStringToken(curtok(p));
            char* t0 = appendstr(c0, t);
            char* t1 = appendstr(t0, c1);
            char* t2 = appendstr(t1, curtxt(p));
            result.success   = false;
            result.error.dsc = appendstr(t2, c2);
            result.error.loc = curloc(p);
            free(t);
            free(t0);
            free(t1);
            free(t2);
          }
        }
        else
          result = thenjdgmt;
      }
      else
      {
        char* c0 = "unexpected missing 'then'; instead saw token:[";
        char* c1 = "], text:[";
        char* c2 = "]";
        char* t  = ToStringToken(curtok(p));
        char* t0 = appendstr(c0, t);
        char* t1 = appendstr(t0, c1);
        char* t2 = appendstr(t1, curtxt(p));
        result.success   = false;
        result.error.dsc = appendstr(t2, c2);
        result.error.loc = curloc(p);
        free(t);
        free(t0);
        free(t1);
        free(t2);
      }
    }
    else
      result = condjdgmt;
  }
  else
  {
    char* c0 = "unexpected missing 'if'; instead saw token:[";
    char* c1 = "], text:[";
    char* c2 = "]";
    char* t  = ToStringToken(curtok(p));
    char* t0 = appendstr(c0, t);
    char* t1 = appendstr(t0, c1);
    char* t2 = appendstr(t1, curtxt(p));
    result.success   = false;
    result.error.dsc = appendstr(t2, c2);
    result.error.loc = curloc(p);
    free(t);
    free(t0);
    free(t1);
    free(t2);
  }
  return result;
}

Judgement ParseIteration(Parser* p)
{
  Location  lhsloc = curloc(p);
  Judgement result;

  if (curtok(p) != T_WHILE)
  {
    char* c0 = "unexpected missing 'while'; instead saw token:[";
    char* c1 = "], text:[";
    char* c2 = "]";
    char* t  = ToStringToken(curtok(p));
    char* t0 = appendstr(c0, t);
    char* t1 = appendstr(t0, c1);
    char* t2 = appendstr(t1, curtxt(p));
    result.success   = false;
    result.error.dsc = appendstr(t2, c2);
    result.error.loc = curloc(p);
    free(t);
    free(t0);
    free(t1);
    free(t2);
    // I could factor out the early returns by
    // assigning to result, and then using GOTOs
    // to jump straight to the 'common' return
    // expression. but that feels evil. more
    // evil than the tower of doom required
    // by the other style of ifs though? (written within
    // conditional).
    return result;
  }
  nextok(p); // eat 'while'

  Judgement condjdgmt = ParseAffix(p);

  if (condjdgmt.success == false)
    return condjdgmt;

  if (curtok(p) != T_DO)
  {
    char* c0 = "unexpected missing 'do'; instead saw token:[";
    char* c1 = "], text:[";
    char* c2 = "]";
    char* t  = ToStringToken(curtok(p));
    char* t0 = appendstr(c0, t);
    char* t1 = appendstr(t0, c1);
    char* t2 = appendstr(t1, curtxt(p));
    result.success   = false;
    result.error.dsc = appendstr(t2, c2);
    result.error.loc = curloc(p);
    free(t);
    free(t0);
    free(t1);
    free(t2);
    return result;
  }
  nextok(p); // eat 'do'

  Judgement bodyjdgmt = ParseAffix(p);

  if (bodyjdgmt.success == false)
    return bodyjdgmt;

  Location  rhsloc = curloc(p);
  Location  itrloc;
  itrloc.first_line   = lhsloc.first_line;
  itrloc.first_column = lhsloc.first_column;
  itrloc.last_line    = rhsloc.first_line;
  itrloc.last_column  = rhsloc.first_column;

  result.success = true;
  result.term    = CreateAstIteration(condjdgmt.term, bodyjdgmt.term, &itrloc);
  return result;
}

Judgement ParseLambda(Parser* p)
{
  Location  lhsloc = curloc(p);
  Judgement result;

  if (curtok(p) != T_BSLASH)
  {
    char* c0 = "unexpected missing '\\'; instead saw token:[";
    char* c1 = "], text:[";
    char* c2 = "]";
    char* t  = ToStringToken(curtok(p));
    char* t0 = appendstr(c0, t);
    char* t1 = appendstr(t0, c1);
    char* t2 = appendstr(t1, curtxt(p));
    result.success   = false;
    result.error.dsc = appendstr(t2, c2);
    result.error.loc = curloc(p);
    free(t);
    free(t0);
    free(t1);
    free(t2);
    return result;
  }
  nextok(p); // eat '\'

  if (curtok(p) != T_IDENTIFIER)
  {
    char* c0 = "unexpected missing 'identifier'; instead saw token:[";
    char* c1 = "], text:[";
    char* c2 = "]";
    char* t  = ToStringToken(curtok(p));
    char* t0 = appendstr(c0, t);
    char* t1 = appendstr(t0, c1);
    char* t2 = appendstr(t1, curtxt(p));
    result.success   = false;
    result.error.dsc = appendstr(t2, c2);
    result.error.loc = curloc(p);
    free(t);
    free(t0);
    free(t1);
    free(t2);
    return result;
  }

  // recall that the string interner copies the
  // text passed in if it does insert, thus we
  // can freely deallocate the memory associated
  // with curtxt (which is allocated by the lexer,
  // and held by the txtbuf, meaning it is deallocated
  // once we try to parse another term.)
  InternedString id = InternString(p->interned_ids, curtxt(p));

  nextok(p); // eat 'identifier'

  // type annotations are required,
  // until we add PolyLambda's, which
  // take over the idea of writing in an
  // annotation free style.
  // (bindings are already annotation free baby B^) )
  if (curtok(p) != T_COLON)
  {
    char* c0 = "unexpected missing ':'; instead saw token:[";
    char* c1 = "], text:[";
    char* c2 = "]";
    char* t  = ToStringToken(curtok(p));
    char* t0 = appendstr(c0, t);
    char* t1 = appendstr(t0, c1);
    char* t2 = appendstr(t1, curtxt(p));
    result.success   = false;
    result.error.dsc = appendstr(t2, c2);
    result.error.loc = curloc(p);
    free(t);
    free(t0);
    free(t1);
    free(t2);
    return result;
  }

  nextok(p); // eat ':'

  Judgement typejdgmt = ParseAffix(p);

  if (typejdgmt.success == false)
  {
    result = typejdgmt;
    return result;
  }
  else if (!is_type_literal(typejdgmt.term))
  {
    char* c0 = "Type annotation must be a Type literal, instead saw term:[";
    char* c1 = "]";
    char* t  = ToStringAst(typejdgmt.term);
    char* t0 = appendstr(c0, t);
    result.success = false;
    result.error.loc = *GetAstLocation(typejdgmt.term);
    result.error.dsc = appendstr(t0, c1);
    free(t);
    free(t0);
    return result;
  }

  if (curtok(p) != T_EQRARROW)
  {
    char* c0 = "unexpected missing '=>'; instead saw token:[";
    char* c1 = "], text:[";
    char* c2 = "]";
    char* t  = ToStringToken(curtok(p));
    char* t0 = appendstr(c0, t);
    char* t1 = appendstr(t0, c1);
    char* t2 = appendstr(t1, curtxt(p));
    result.success   = false;
    result.error.dsc = appendstr(t2, c2);
    result.error.loc = curloc(p);
    free(t);
    free(t0);
    free(t1);
    free(t2);
    return result;
  }
  nextok(p);

  // so, we need to be aware of the language
  // semantics within the parser itself here.
  // while we parse the body of this procedure,
  // we need to point the outer scope to the
  // correct place, in order to make the name
  // Lookup rules work: we need to point the
  // symbols to this lambda's symbol table
  // so that if a lambda is created within the
  // body of this lambda, the symbol tables
  // form a continuous chain from the
  // innermost procedure outwards to global scope.
  // also also, this is essentially how i believe
  // we can handle loops and conditionals having
  // their own scope, they hold a scope, and we
  // connect it to the outer scope.
  // this is maintaining the static link between
  // all living activation records essentially.
  SymbolTable* localsymbols = CreateSymbolTable(p->symbols);
  SymbolTable* lastsymbols  = p->symbols;
  p->symbols                = localsymbols;

  ScopeSet localscope = IntroduceNewScope(p->scope, p->scope);
  ScopeSet lastscope  = p->scope;
  p->scope            = localscope;

  Judgement bodyjdgmt = ParseAffix(p);

  if (bodyjdgmt.success == false)
    return bodyjdgmt; // TODO: if we actually take this path we gonna leak!

  p->symbols      = lastsymbols; // reset the outer scope to what it was before
  p->scope        = lastscope;

  Location  rhsloc    = curloc(p);
  Location  lamloc;
  lamloc.first_line   = lhsloc.first_line;
  lamloc.first_column = lhsloc.first_column;
  lamloc.last_line    = rhsloc.first_line;
  lamloc.last_column  = rhsloc.first_column;

  result.success      = true;
  result.term         = CreateAstLambda(id, typejdgmt.term, bodyjdgmt.term, localsymbols, localscope, &lamloc);
  return result;
}

Judgement ParseUnop(Parser* p)
{
  Judgement result;
  if (curtok(p) != T_OPERATOR)
  {
    char* c0 = "unexpected missing 'operator'; instead saw token:[";
    char* c1 = "], text:[";
    char* c2 = "]";
    char* t  = ToStringToken(curtok(p));
    char* t0 = appendstr(c0, t);
    char* t1 = appendstr(t0, c1);
    char* t2 = appendstr(t1, curtxt(p));
    result.success   = false;
    result.error.dsc = appendstr(t2, c2);
    result.error.loc = curloc(p);
    free(t);
    free(t0);
    free(t1);
    free(t2);
    return result;
  }

  if (!is_unop(p, curtxt(p)))
  {
    char* c0 = "operator [";
    char* c1 = "] not bound as unop within the environment";
    char* t0 = appendstr(c0, curtxt(p));
    result.success   = false;
    result.error.dsc = appendstr(t0, c1);
    result.error.loc = curloc(p);
    free(t0);
    return result;
  }

  Location  lhsloc = curloc(p);
  char*     copy   = curtxt(p);
  char*     optxt  = dupnstr(copy, strlen(copy));

  nextok(p); // eat operator

  Judgement rhsjdgmt = ParsePrimary(p);

  if (rhsjdgmt.success == false)
    return rhsjdgmt;

  Location  rhsloc    = curloc(p);
  Location  uoploc;
  uoploc.first_line   = lhsloc.first_line;
  uoploc.first_column = lhsloc.first_column;
  uoploc.last_line    = rhsloc.first_line;
  uoploc.last_column  = rhsloc.first_column;

  InternedString op      = InternString(p->interned_ops, optxt);

  result.success         = true;
  result.term            = CreateAstUnop(op, rhsjdgmt.term, &uoploc);
  return result;
}


// this is an implementation of an operator precedence parser.
//
Judgement ParseInfix(Parser* p, Judgement left, int min_prec)
{
  if (left.success == false)
    return left;

  Location        lhsloc    = curloc(p);
  BinopPrecAssoc* lookahead = NULL;

  if (curtok(p) != T_OPERATOR)
  {
    char* c0 = "unexpected missing 'operator'; instead saw token:[";
    char* c1 = "], text:[";
    char* c2 = "]";
    char* t  = ToStringToken(curtok(p));
    char* t0 = appendstr(c0, t);
    char* t1 = appendstr(t0, c1);
    char* t2 = appendstr(t1, curtxt(p));
    left.success   = false;
    left.error.dsc = appendstr(t2, c2);
    left.error.loc = curloc(p);
    free(t);
    free(t0);
    free(t1);
    free(t2);
    return left;
  }

  if (!is_binop(p, curtxt(p)))
  {
    char* c0 = "operator [";
    char* c1 = "] is not bound as binop in environment";
    char* t0 = appendstr(c0, curtxt(p));
    left.success   = false;
    left.error.dsc = appendstr(t0, c1);
    left.error.loc = lhsloc;
    free(t0);
    return left;
  }

  InternedString Iop;

  // since we intern operators now, we have to account for that
  // when parsing them
  while ((Iop = InternString(p->interned_ops, curtxt(p)))
     && (lookahead = FindBinopPrecAssoc(p->precedence_table, Iop)) && lookahead->precedence >= min_prec)
  {

    Location lhsloc = curloc(p), rhsloc;

    BinopPrecAssoc* operator = lookahead;

    nextok(p); // eat 'operator'
    // we don't recurr fully up the grammr, because dont we want
    // to recurr into this procedure again, so we go up
    // to application, one step below where infix procedures can
    // be parsed.
    Judgement right = ParseApplication(p);

    if (right.success == false)
      return right;

    // after the right hand side of the last operator
    // was parsed, if we see another operator we need to choose
    // how to handle that, either we need to parse at a now
    // higher precedence, which we use recursion to express,
    // or we continue parsing
    if (curtok(p) == T_OPERATOR)
    {
      if (!is_binop(p, curtxt(p)))
      {
        char* c0 = "operator [";
        char* c1 = "] is not bound as binop in environment";
        char* t0 = appendstr(c0, curtxt(p));
        left.success   = false;
        left.error.dsc = appendstr(t0, c1);
        left.error.loc = curloc(p);
        free(t0);
        return left;
      }


      InternedString Irop;

      while((Irop = InternString(p->interned_ops, curtxt(p)))
         && (lookahead = FindBinopPrecAssoc(p->precedence_table, Irop))
         && (lookahead->precedence > operator->precedence
           || (lookahead->precedence == operator->precedence && lookahead->associativity == A_RIGHT)))
      {
        right = ParseInfix(p, right, lookahead->precedence);

        if (right.success == false)
          return right;
      }

      rhsloc = curloc(p);
    }

    Location boploc;
    boploc.first_line   = lhsloc.first_line;
    boploc.first_column = lhsloc.first_column;
    boploc.last_line    = rhsloc.last_line;
    boploc.last_column  = rhsloc.last_column;

    left.success = true;
    left.term    = CreateAstBinop(Iop, left.term, right.term, &boploc);
  }
  return left;
}

Judgement Parse(Parser* p, char* input)
{
  ResetParser(p);

  yySetBuffer(p->lexer, input, strlen(input));

  filltok(p, 1);

  // enter into the top of the parser.
  Judgement result = ParseAffix(p);
  return result;
}
