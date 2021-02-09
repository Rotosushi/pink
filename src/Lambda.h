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

void InitializeLambda(Lambda* lambda, InternedString arg_id, struct Type* arg_type, struct Ast* body, struct SymbolTable* scope);

void DestroyLambda(Lambda* lam);

void CloneLambda(Lambda* destination, Lambda* source);

char* ToStringLambda(Lambda* lam);

TypeJudgement Getype(Lambda* lam, Environment* env);

#endif // !LAMBDA_H
