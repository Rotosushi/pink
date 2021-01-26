#include <stdlib.h>
#include <string.h>

#include "Ast.h"
#include "Type.h"
#include "StringInterner.h"
#include "SymbolTable.h"
#include "Lambda.h"

void DestroyLambda(Lambda* lam)
{
  DestroyType(lam->arg_type);
  DestroyAst(lam->body);
  DestroySymbolTable(lam->scope);
  free(lam);
  lam = NULL;
}

Lambda* CloneLambda(Lambda* lam)
{
  Lambda* result   = (Lambda*)malloc(sizeof(Lambda));
  result->arg_id   = lam->arg_id;
  result->arg_type = CloneType(lam->arg_type);
  result->body     = CloneAst(lam->body);
  result->scope    = CloneSymbolTable(lam->scope);
  return result;
}

char* ToStringLambda(Lambda* lam)
{
  char *result, *argtype, *body;
  char *bslsh, *cln, *eqrarrow;
  bslsh    = "\\"; // 1
  cln      = ": "; // 2
  eqrarrow = " => "; // 4
  argtype  = ToStringType(lam->arg_type);
  body     = ToStringAst(lam->body);

  // 7 extra characters and a null terminator walk into a string...
  int sz = strlen(argtype) + strlen(body) + 8;
  result = (char*)calloc(sz, sizeof(char));

  strcat(result, bslsh);
  strcat(result, (char*)lam->arg_id);
  strcat(result, cln);
  strcat(result, argtype);
  strcat(result, eqrarrow);
  strcat(result, body);
  return result;
}
