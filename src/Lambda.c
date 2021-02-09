#include <stdlib.h>
#include <string.h>

#include "Ast.h"
#include "Type.h"
#include "StringInterner.h"
#include "SymbolTable.h"
#include "Lambda.h"

void InitializeLambda(Lambda* lambda, InternedString arg_id, struct Type* arg_type, struct Ast* body, struct SymbolTable* scope)
{
  lambda->arg_id   = arg_id;
  lambda->arg_type = arg_type;
  lambda->body     = body;
  lambda->scope    = scope;
}

void DestroyLambda(Lambda* lam)
{
  DestroyAst(lam->body);
  DestroySymbolTable(lam->scope);
}

void CloneLambda(Lambda* destination, Lambda* source)
{
  destination->arg_id   = source->arg_id
  destination->arg_type = source->arg_type;

  CloneAst(&(destination->body), source->body);

  CloneSymbolTable(&(destination->scope), source->scope);
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

TypeJudgement Getype(Lambda* lam, Environment* env)
{
  TypeJudgement result;

  

  return result;
}
