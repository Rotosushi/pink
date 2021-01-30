#include <stdlib.h>
#include <string.h>

#include "Ast.h"
#include "StringInterner.h"
#include "Binop.h"

Binop* CreateBinop(InternedString op, struct Ast* left, struct Ast* right)
{
  Binop* result = (Binop*)malloc(sizeof(Binop));
  result->op    = op;
  result->lhs   = left;
  result->rhs   = right;
  return result;
}

void DestroyBinop(Binop* binop)
{
  DestroyAst(binop->lhs);
  DestroyAst(binop->rhs);
  free(binop);
  binop = NULL;
}

Binop* CloneBinop(Binop* binop)
{
  Binop* result = (Binop*)malloc(sizeof(Binop));
  result->op  = binop->op;
  result->lhs = CloneAst(binop->lhs);
  result->rhs = CloneAst(binop->rhs);
  return result;
}

char* ToStringBinop(Binop* binop)
{
  char *result, *spc, *left, *right;
  spc   = " ";
  left  = ToStringAst(binop->lhs);
  right = ToStringAst(binop->rhs);

  int sz = strlen((char*)binop->op)
         + strlen(left)
         + strlen(right)
         + 3; // ...2 spaces and a null terminator walk into this string
  result = (char*)calloc(sz, sizeof(char));

  strcat(result, left);
  strcat(result, spc);
  strcat(result, (char*)binop->op);
  strcat(result, spc);
  strcat(result, right);
  return result;
}
