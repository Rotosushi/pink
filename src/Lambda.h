#ifndef LAMBDA_H
#define LAMBDA_H

#include "StringInterner.h"

struct Ast;
struct Type;
struct SymbolTable;

typedef struct Lambda
{
  InternedString arg_id;
  struct Type*   arg_type;
  struct Ast*    body;
  struct SymbolTable*   scope;
} Lambda;

void DestroyLambda(Lambda* lam);

Lambda* CloneLambda(Lambda* lam);

char* ToStringLambda(Lambda* lam);

#endif // !LAMBDA_H
