#include <stdlib.h>
#include <string.h>

#include "Ast.h"
#include "StringInterner.h"
#include "Unop.h"

void DestroyUnop(Unop* unop)
{
  DestroyAst(unop->rhs);
  free(unop);
}

Unop* CloneUnop(Unop* unop)
{
  Unop* result = (Unop*)malloc(sizeof(Unop));
  result->op   = unop->op;
  result->rhs  = CloneAst(unop->rhs);
  return result;
}

char* ToStringUnop(Unop8 unop)
{
  char *result, *spc, *right;
  spc   = " ";
  right = ToStringAst(unop->rhs);

  int sz = strlen((char*)unop->op) + strlen(right) + 2;
  result = (char*)calloc(sz, sizeof(char));

  strcat(result, (char*)unop->op);
  strcat(result, spc);
  strcat(result, right);
  return result;
}
