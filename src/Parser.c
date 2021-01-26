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

#include "StringInterner.h"
#include "ParseJudgement.h"
#include "Parser.h"


struct Parser* CreateParser(StringInterner* Iids, StringInterner* Iops, BinopPrecedenceTable* prec_table, BinopTable* bs, UnopTable* us);
{
  Parser* result    = (Parser*)malloc(sizeof(Parser));
  result->lexer     = (Lexer*)malloc(sizeof(Lexer));
  result->markstack = NULL;
  result->tokbuf    = NULL;
  result->txtbuf    = NULL;
  result->locbuf    = NULL;
  result->idx       = 0;
  result->mkstksz   = 0;
  result->bufsz     = 0;
  result->interned_ids = Iids;
  result->interned_ops = Iops;
  result->precedence_table = prec_table;
  result->binops = bs;
  result->unops  = us;
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

void DumpTokens(Parser* p, FILE* out)
{
  for (int i = 0; i < p->bufsz; i++)
  {
    fprintf(out, "[%s]", ToStringToken(p->tokbuf[i]));
  }
  fprintf(out, "\n");
}

int mark(Parser* p)
{
  /*
  push the current index onto the mark 'stack'

  ms = [idx0, idx1, ..., idxn]

  ~> ms = [idx0, idx1, ..., idxn, idxn+1]
  */
  if (p->markstack == NULL)
  {
    p->markstack = (int*)malloc(sizeof(int));
    p->mkstksz   = 1;
  }
  else
  {
    p->mkstksz += 1;
    p->markstack = (int*)realloc(p->markstack, sizeof(int) * p->mkstksz);
  }

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

    p->tokbuf = (Token*)realloc(p->tokbuf, sizeof(Token) * (p->bufsz + n));
    p->txtbuf = (char**)realloc(p->txtbuf, sizeof(char*) * (p->bufsz + n));
    p->locbuf = (Location*)realloc(p->locbuf, sizeof(Location) * (p->bufsz + n));

    for (int j = 0; j < n; j++)
    {
      Token t = T_ERR;

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
    return T_ERR;

  return &(p->locbuf[p->idx]);
}

bool is_unop(Parser* p, char* input)
{
  // this procedure may have more to do
  // now that we intern operators?
}

bool is_binop(Parser* p, char* input)
{

}

bool is_primary(Token t)
{

}

bool is_type_primary(Token t)
{

}

bool is_ender(Token t)
{

}


/*
affix := primary
       | primary operator primary
       | primary "<-" affix
*/

ParseJudgement ParseAffix(Parser* p)
{
  Location lhsloc = *(curloc(p));
  Ast*    lhsterm = ParsePrimary(p);

  if (curtok(p) == T_OPERATOR)
  {
    lhsterm = ParseInfix(p, lhsterm, 0);
  }
  else if (curtok(p) == T_LARROW)
  {
    nextok(p); // eat '<-'
    Ast* rhsterm = ParseAffix(p);

  }
}

ParseJudgement ParsePrimary(Parser* p);
ParseJudgement ParsePrimitive(Parser* p);
ParseJudgement ParseIf(Parser* p);
ParseJudgement ParseWhile(Parser* p);
ParseJudgement ParseProcedure(Parser* p);
ParseJudgement ParseInfix(Parser* p, Ast* lhs, int min_prec);


ParseJudgement Parse(Parser* p, char* input)
{

}
