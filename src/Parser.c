/*
  the grammar so far

  term := affix
        | affix ';' term

  affix := primary
         | primary operator primary
         | primary "<-" affix

  primary := primitive
           | primitive primitive

  primitive := type
             | atom
             | unop atom

  atom := identifier
        | identifier := term
        | nil
        | [0-9]+
        | true
        | false
        | \ identifier (: term)? => term
        | 'if' term 'then' term 'else' term
        | 'while' term 'do' term
        | '(' term ')'

  type := "Nil"
        | "Int"
        | "Bool"
        | "Poly"
        | "Ref" type
        | type -> type

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

#include "SymbolTable.h"
#include "StringInterner.h"
#include "Ast.h"
#include "ParseJudgement.h"
#include "TypeJudgement.h"
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


struct Parser* CreateParser(SymbolTable* global_scope, StringInterner* Iids, StringInterner* Iops, BinopPrecedenceTable* prec_table, BinopTable* bs, UnopTable* us);
{
  Parser* result    = (Parser*)malloc(sizeof(Parser));
  result->lexer     = CreateLexer(Iids, Iops);
  result->markstack = NULL;
  result->tokbuf    = NULL;
  result->txtbuf    = NULL;
  result->locbuf    = NULL;
  result->idx       = 0;
  result->mkstksz   = 0;
  result->bufsz     = 0;
  result->outer_scope      = global_scope;
  result->interned_ids     = Iids;
  result->interned_ops     = Iops;
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
// write the output data.
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

Location* curloc(Parser* p)
{
  if (p->locbuf == NULL)
    return NULL;

  return &(p->locbuf[p->idx]);
}

bool is_unop(Parser* p, InternedString input)
{
  InternedString possible = FindUnop(p->unops, input);

  if (possible != NULL)
    return true;
  else
    return false;
}

bool is_binop(Parser* p, InternedString input)
{
  InternedString possible = FindBinop(p->binops, input);

  if (possible != NULL)
    return true;
  else
    return false;
}

bool is_primary(Token t)
{
  switch (t)
  {
    case T_INTEGER: case T_TYPE_INT:
    case T_TRUE: case T_FALSE: case T_TYPE_BOOL:
    case T_NIL: case T_TYPE_NIL:
    case T_IDENTIFIER: case T_IF: case T_WHILE:
    case T_LPAREN: case T_BSLASH:
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

ParseJudgement ParseAffix(Parser* p);
ParseJudgement ParseApplication(Parser* p);
ParseJudgement ParsePrimitive(Parser* p);
TypeJudgement  ParseType(Parser* p);  // asymmetry for convenience!
ParseJudgement ParseConditional(Parser* p);
ParseJudgement ParseIteration(Parser* p);
ParseJudgement ParseLambda(Parser* p);
ParseJudgement ParseInfix(Parser* p, Ast* lhs, int min_prec);

/*
affix := application
       | application operator application
       | application "<-" affix
       // | application "." affix
*/
ParseJudgement ParseAffix(Parser* p)
{
  Location*      lhsloc = NULL;
  ParseJudgement lhsjdgmt;
  lhsjdgmt.success = false;
  if (is_primary(curtok(p)))
  {
    lhsloc   = curloc(p);
    lhsjdgmt = ParseApplication(p);
  }

  if (lhsjdgmt.success == true)
  {
    if (curtok(p) == T_OPERATOR)
    {
      lhsterm = ParseInfix(p, lhsterm, 0);
    }
    else if (curtok(p) == T_LARROW)
    {
      nextok(p); // eat '<-'
      ParseJudgement rhsjdgmt = ParseAffix(p);

      if (rhsjdgmt.success = true)
      {
        Location*   rhsloc       = curloc(p);
        Assignment* assignment   = CreateAssignment(lhsjdgmt.term, rhsjdgmt.term);

        Ast*        result       = (Ast*)malloc(sizeof(Ast));
        result->kind             = A_ASS;
        result->loc.first_line   = lhsloc->first_line;
        result->loc.first_column = lhsloc->first_column;
        result->loc.last_line    = rhsloc->first_line;
        result->loc.last_column  = rhsloc->first_column;
        result->ass              = assignment;

        lhsjdgmt.term            = result;
        return lhsjdgmt;
      }
      else
        return rhsjdgmt;
    }
  }
  else
    return lhsjdgmt;
}

/*
application := primitive
             | primitive application
*/
ParseJudgement ParseApplication(Parser* p)
{
  Location*      lhsloc   = curloc(p);
  ParseJudgement lhsjdgmt = ParsePrimitive(p);

  if (lhsjdgmt.success == true)
  {
    if (is_primary(curtok(p)))
    {
      ParseJudgement rhsjdgmt = ParseApplication(p);

      if (rhsjdgmt.success == true)
      {
        Location*      rhsloc    = curloc(p);
        Application*   app       = CreateApplication(lhsjdgmt.term, rhsjdgmt.term);
        Ast*           result    = (Ast*)malloc(sizeof(Ast));
        result->kind             = A_APP;
        result->loc.first_line   = lhsloc->first_line;
        result->loc.first_column = lhsloc->first_column;
        result->loc.last_line    = rhsloc->last_line;
        result->loc.last_column  = rhsloc->last_column;
        result->app              = app;
        lhsjdgmt.term            = result;
        return lhsjdgmt; // (1)
      }
      else
        return rhsjdgmt;
    }
    else
      return lhsjdgmt; // (2)
  }
  else
    return lhsjdgmt; // (3)

  // (1, 2, 3) could all be removed and replaced with:
  // return lhsjdgmt;
  // right here, and i think the code would be less
  // legible.
}

/*
primitive := type
           | atom
           | unop atom
*/
ParseJudgement ParsePrimitive(Parser* p)
{
  switch (curtok(p))
  {
    case T_TYPE_NIL: case T_TYPE_INT: case T_TYPE_BOOL:
      {
        ParseJudgement result;
        TypeJudgement  type = ParseType(p);

        if (type.success == true)
        {
          Object* obj = (Object*)malloc(sizeof(Object));
          obj->kind   = O_TYPE;
          obj->type   = type.term;
          Ast* ast    = (Ast*)malloc(sizeof(Ast));
          ast->kind   = A_OBJ;
          ast->obj    = obj;
          result.success = true;
          result.term    = ast;
        }
        else
        {
          result.success = false;
          result.error   = type.error;
        }
        return result;
        break;
      }


    case T_IDENTIFIER: case T_NIL: case T_INTEGER:
    case T_TRUE: case T_FALSE: case T_BSLASH:
    case T_IF: case T_WHILE: case T_LPAREN:
      return ParseAtom(p);
      break;

    case T_OPERATOR:
      return ParseUnop(p);
      break;

    default:
      Location*      errloc = curloc(p);
      ParseJudgement result;
      result.success   = false;
      result.error.dsc = "unknown primitive token found";
      result.error.loc = *errloc;
      return result;
  }
}

/*
atom := identifier
      | identifier := term
      | nil
      | [0-9]+
      | true
      | false
      | \ identifier (: term)? => term
      | 'if' term 'then' term 'else' term
      | 'while' term 'do' term
      | '(' term ')'
*/
ParseJudgement ParseAtom(Parser* p)
{
  Location* lhsloc = curloc(p);
  ParseJudgement lhsjdgmt;

  switch(curtok(p))
  {
    case T_IDENTIFIER:
    {
      // the StringInterner itself disconnects the string we pass in
      // from the buffer of strings we are holding in the parser.
      // by making a copy of each string we pass in to hold for itself
      InternedString id = InternString(p->interned_ids, curtxt(p));
      nextok(p); // eat T_IDENTIFIER

      if (curtok(p) == T_COLONEQ)
      {
        nextok(p); // eat T_COLONEQ
        ParseJudgement rhsjdgmt = ParseAffix(p);

        if (rhsjdgmt.success == true)
        {
          Location* rhsloc         = curloc(p);
          Bind* bind               = CreateBind(id, rhsjdgmt.term);
          Ast* result              = (Ast*)malloc(sizeof(Ast));
          result->kind             = A_BND;
          result->loc.first_line   = lhsloc->first_line;
          result->loc.first_column = lhsloc->first_column;
          result->loc.last_line    = rhsloc->first_line;
          result->loc.last_column  = rhsloc->first_column;
          result->bnd              = bind;
          lhsjdgmt.term            = result;
          return lhsjdgmt;
        }
        else
          return rhsjdgmt;
      }
      else
      {
        Variable* variable = CreateVariable(id);
        Ast* result   = (Ast*)malloc(sizeof(Ast));
        result->kind  = A_VAR;
        result->loc   = *lhsloc;
        result->var   = variable;
        lhsjdgmt.term = result;
        return lhsjdgmt;
      }
      break;
    }
    case T_NIL:
    {
      nextok(p); // eat "nil"
      Nil*    nil   = CreateNil();
      Object* obj   = (Object*)malloc(sizeof(Object));
      obj->kind     = O_NIL;
      obj->nil      = nil;
      Ast* result   = (Ast*)malloc(sizeof(Ast));
      result->kind  = A_OBJ;
      result->loc   = *lhsloc;
      result->obj   = obj;
      lhsjdgmt.term = result;
      return lhsjdgmt;
      break;
    }
    case T_INTEGER:
    {
      nextok(p); // eat Integer
      Integer* integer = CreateInteger(atoi(curtxt));
      Object*  obj     = (Object*)malloc(sizeof(Object));
      obj->kind        = O_INT;
      obj->integer     = integer;
      Ast* result      = (Ast*)malloc(sizeof(Ast));
      result->kind     = A_OBJ;
      result->loc      = *lhsloc;
      result->obj      = obj;
      lhsjdgmt.term    = result;
      return lhsjdgmt;
      break;
    }
    case T_TRUE:
    {
      nextok(p); // eat "true"
      Boolean* boolean = CreateBoolean(true);
      Object*  obj     = (Object*)malloc(sizeof(Object));
      // TODO: i am sure we can support a type polymorphic
      // constructor procedure with the signature
      // Create(Typename, ...);
      obj->kind        = O_BOOL;
      obj->boolean     = boolean;
      Ast* result      = (Ast*)malloc(sizeof(Ast));
      result->kind     = A_OBJ;
      result->loc      = *lhsloc;
      result->obj      = obj;
      lhsjdgmt.term    = result;
      return lhsjdgmt;
      break;
    }
    case T_FALSE:
    {
      nextok(p); // eat "false"
      Boolean* boolean = CreateBoolean(false);
      Object*  obj     = (Object*)malloc(sizeof(Object));
      obj->kind        = O_BOOL;
      obj->boolean     = boolean;
      Ast* result      = (Ast*)malloc(sizeof(Ast));
      result->kind     = A_OBJ;
      result->loc      = *lhsloc;
      result->obj      = obj;
      lhsjdgmt.term    = result;
      return lhsjdgmt;
      break;
    }
    case T_BSLASH:
    {
      lhsjdgmt = ParseLambda(p);
      return lhsjdgmt;
      break;
    }
    case T_IF:
    {
      lhsjdgmt = ParseIf(p);
      return lhsjdgmt;
      break;
    }
    case T_WHILE:
    {
      lhsjdgmt = ParseWhile(p);
      break;
    }
    case T_LPAREN:
    {
      break;
    }
    default:
    {
      Location*      errloc = curloc(p);
      ParseJudgement result;
      result.success   = false;
      result.error.dsc = "unknown Atom token found";
      result.error.loc = *errloc;
      return result;
    }
  }
}

/*
type := "Nil"
      | "Int"
      | "Bool"
    //  | "Poly"
    //  | "Ref" type
      | type -> type
*/
TypeJudgement ParseTypeAtom(Parser* p)
{
  Location*      lhsloc = curloc(p);
  TypeJudgement  result;
  switch (curtok(p))
  {
    case T_TYPE_INT:
    {
      nextok(p);
      ScalarType* scalarType = CreateScalarType(S_INT);
      Type* type             = (Type*)malloc(sizeof(Type));
      type->kind             = T_SCALAR;
      type->scalar           = scalarType;
      result.success         = true;
      result.term            = type;
      break;
    }
    case T_TYPE_NIL:
    {
      nextok(p);
      ScalarType* scalarType = CreateScalarType(S_NIL);
      Type* type             = (Type*)malloc(sizeof(Type));
      type->kind             = T_SCALAR;
      type->scalar           = scalarType;
      result.success         = true;
      result.term            = type;
      break;
    }

    case T_TYPE_BOOL:
    {
      nextok(p);
      ScalarType* scalarType = CreateScalarType(S_BOOL);
      Type* type             = (Type*)malloc(sizeof(Type));
      type->kind             = T_SCALAR;
      type->scalar           = scalarType;
      result.success         = true;
      result.term            = type;
      break;
    }

    default:
    {
      result.success   = false;
      result.error.dsc = "unknown Type Atom token found";
      result.error.loc = *lhsloc;
      break;
    }
  }
  return result;
}

TypeJudgement ParseType(Parser* p)
{
  Location* lhsloc = curloc(p);
  TypeJudgement lhsjdgmt = ParseTypeAtom(p);

  if (lhsjdgmt.success = true)
  {
    if (curtok(p) == T_RARROW)
    {
      nextok(p);
      TypeJudgement rhsjdgmt = ParseType(p);

      if (rhsjdgmt.success == true)
      {
        ProcType* pt  = CreateProcType(lhsjdgmt.term, rhsjdgmt.term);
        Type* type    = (Type*)malloc(sizeof(Type));
        type->kind    = T_PROC;
        type->proc    = pt;
        lhsjdgmt.term = type;
        return lhsjdgmt;
      }
      else
        return rhsjdgmt;
    }
    else
      return lhsjdgmt;
  }
  else
    return lhsjdgmt;
}

/*
  I think, regarding the dangling else
  problem, i like the solution of distinguishing
  the single alternative conditional and the
  double alternative conditional syntactical.
*/
ParseJudgement ParseConditional(Parser* p)
{
  Location* lhsloc = curloc(p);
  ParseJudgement result;

  if (curtok(p) == T_IF)
  {
    nextok(p); // eat 'if'

    ParseJudgement condjdgmt = ParseAffix(p);

    if (condjdgmt.success == true)
    {
      if (curtok(p) == T_THEN)
      {
        nextok(p); // eat 'then'

        ParseJudgement thenjdgmt = ParseAffix(p);

        if (thenjdgmt.success == true)
        {
          if (curtok(p) == T_ELSE)
          {
            nextok(p); // eat 'else'

            ParseJudgement elsejdgmt = ParseAffix(p);

            if (elsejdgmt.success == true)
            {
              Location* rhsloc = curloc(p);
              Conditional* cndl = (Conditional*)malloc(sizeof(Conditional));
              cndl->cnd = condjdgmt.term;
              cndl->fst = thenjdgmt.term;
              cndl->snd = elsejdgmt.term;
              Ast* ast  = (Ast*)malloc(sizeof(Ast));
              ast->kind = A_CND;
              ast->loc.first_line   = lhsloc->first_line;
              ast->loc.first_column = lhsloc->first_column;
              ast->loc.last_line    = rhsloc->first_line;
              ast->loc.last_column  = rhsloc->first_column;
              ast->cnd  = cndl;
              result.success = true;
              result.term = ast;
            }
            else
              result = elsejdgmt;
          }
          else
          {
            result.success = false;
            result.error.dsc = "unexpected missing 'else'";
            result.error.loc = *(curloc(p));
          }
        }
        else
          result = thenjdgmt;
      }
      else
      {
        result.success = false;
        result.error.dsc = "unexpected missing 'then'";
        result.error.loc = *(curloc(p));
      }
    }
    else
      result = condjdgmt;
  }
  else
  {
    result.success   = false;
    result.error.dsc = "unexpected missing 'if'";
    result.error.loc = *(curloc(p));
  }
  return result;
}

ParseJudgement ParseIteration(Parser* p)
{
  Location* lhsloc = curloc(p);
  ParseJudgement result;

  if (curtok(p) != T_WHILE)
  {
    result.success   = false;
    result.error.dsc = "unexpected missing while";
    result.error.loc = *(curloc(p));
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

  ParseJudgement condjdgmt = ParseAffix(p);

  if (condjdgmt.success == false)
    return condjdgmt;

  if (curtok(p) != T_DO)
  {
    result.success   = false;
    result.error.dsc = "unexpected missing 'do'";
    result.error.loc = *(curloc(p));
    return result;
  }
  nextok(p); // eat 'do'

  ParseJudgement bodyjdgmt = ParseAffix(p);

  if (bodyjdgmt.success == false)
    return bodyjdgmt;

  Location* rhsloc = curloc(p);

  Iteration* itr = (Iteration*)malloc(sizeof(Iteration));
  itr->cnd = condjdgmt.term;
  itr->bdy = condjdgmt.term;
  Ast* ast = (Ast*)malloc(sizeof(Ast));
  ast->kind = A_ITR;
  ast->loc.first_line   = lhsloc->first_line;
  ast->loc.first_column = lhsloc->first_column;
  ast->loc.last_line    = rhsloc->first_line;
  ast->loc.last_column  = rhsloc->first_column;
  ast->itr  = itr;
  result.success = true;
  result.term    = ast;
  return result;
}

ParseJudgement ParseLambda(Parser* p)
{
  Location* lhsloc = curloc(p);
  ParseJudgement result;

  if (curtok(p) != T_BSLASH)
  {
    result.success   = false;
    result.error.dsc = "unexpected missing '\\'";
    result.error.loc = *lhsloc;
    return result;
  }
  nextok(p); // eat '\'

  if (curtok(p) != T_ID)
  {
    result.success   = false;
    result.error.dsc = "unexpected missing identifier";
    result.error.loc = *curloc(p);
    return result;
  }

  InternedString id = InternString(p->interned_ids, curtxt(p));

  nextok(p); // eat 'identifier'

  if (curtok(p) != T_COLON)
  {
    result.success   = false;
    result.error.dsc = "unexpected missing ':'";
    result.error.loc = *curloc(p);
    return result;
  }

  nextok(p); // eat ':'

  TypeJudgement typejdgmt = ParseType(p);

  if (typejdgmt.success == false)
  {
    result.success = false;
    result.error   = typejdgmt.error;
    return result;
  }

  if (curtok(p) != T_EQRARROW)
  {
    result.success   = false;
    result.error.dsc = "unexpected missing '=>'";
    result.error.lco = *(curloc(p));
    return result;
  }
  nextok(p);

  // so, we need to be aware of the language
  // semantics within the parser itself here.
  // while we parse the body of this procedure,
  // we need to point the outer scope to the
  // correct place, in order to make the name
  // lookup rules work. we need to point the
  // current_scope to this lambda's symbol table
  // so that if a lambda is created within the
  // body of this lambda, the symbol tables
  // form a continuous chain from the
  // innermost procedure outwards to global scope.
  SymbolTable* lambda_scope = CreateSymbolTable(p->outer_scope);
  SymbolTable* last_scope   = p->outer_scope;
  p->outer_scope            = lambda_scope;

  ParseJudgement bodyjdgmt = ParseAffix(p);

  if (bodyjdgmt.success == false)
    return bodyjdgmt; // if we actually take this path we gonna leak!

  Location* rhsloc = curloc(p);

  Lambda* lam    = (Lambda*)malloc(sizeof(Lambda));
  lam->arg_id    = id;
  lam->arg_type  = typejdgmt.type;
  lam->body      = bodyjdgmt.term;
  lam->scope     = lambda_scope;
  p->outer_scope = last_scope; // reset the outer scope to what it was before
  Object* obj    = (Object*)malloc(sizeof(Object));
  obj->kind      = O_LAMB;
  obj->lambda    = lam;
  Ast* ast       = (Ast*)malloc(sizeof(Ast));
  ast->kind      = A_OBJ;
  ast->loc.first_line   = lhsloc->first_line;
  ast->loc.first_column = lhsloc->first_column;
  ast->loc.last_line    = rhsloc->first_line;
  ast->loc.last_column  = rhsloc->first_column;
  ast->obj       = obj;
  return result;
}

ParseJudgement ParseInfix(Parser* p, Ast* lhs, int min_prec)
{
  ParseJudgement result;
  BinopPrecAssoc* lookahead = NULL;

  if (curtok(p) != T_OPERATOR)
  {
    result.success   = false;
    result.error.dsc = "unexpected missing operator";
    result.error.loc = *lhsloc;
    return result;
  }

  while ((lookahead = FindBinopPrecAssoc(p->precedence_table, curtxt(p))) && lookahead->precedence >= min_prec)
  {
    Location* lhsloc = curloc(p);
    char* optxt = curtxt(p);
    BinopPrecAssoc* operator = lookahead;

    nextok(p); // eat 'operator'
    ParseJudgement right = ParseApplication(p);

    if (right.success == false)
      return right;

    Location* rhsloc = curloc(p);

    while((lookahead = FindBinopPrecAssoc(p->precedence_table, curtxt(p)))
       && (lookahead->precedence > operator->precedence
       || (lookahead->precedence == operator->precedence && lookahead->associativity == A_RIGHT)))
    {
      right = ParseInfix(right.term, lookahead->precedence);

      if (right.success == false)
        return right;
    }

    Binop* binop = (Binop*)malloc(sizeof(Binop));
    binop->op = InternString(p->interned_ops, optxt);
    binop->lhs = lhs;
    binop->rhs = right.term;
    Ast* ast = (Ast*)malloc(sizeof(Ast));
    ast->kind = A_BNP;
    ast->bnp  = binop;
    result.success = true;
    result.term    = ast;
  }
  return result;
}

ParseJudgement Parse(Parser* p, char* input)
{
  ResetParser(p);

  yySetBuffer(p->lexer, input, strlen(input));

  filltok(p, 1);

  ParseJudgement result = ParseAffix(p);
  return result;
}
