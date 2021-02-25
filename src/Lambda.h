#ifndef LAMBDA_H
#define LAMBDA_H

#include "TypeJudgement.h"
#include "StringInterner.h"
#include "Location.h"
struct Ast;
struct SymbolTable;
struct Environment;

typedef struct Lambda
{
  Location            loc;
  InternedString      arg_id;
  struct Ast*         arg_type;
  struct Ast*         body;
  struct SymbolTable* scope;
} Lambda;

void InitializeLambda(Lambda* lambda, InternedString arg_id, struct Ast* arg_type, struct Ast* body, struct SymbolTable* scope, Location* loc);

void DestroyLambda(Lambda* lam);

void CloneLambda(Lambda* destination, Lambda* source);

char* ToStringLambda(Lambda* lam);

TypeJudgement GetypeLambda(Lambda* node, struct Environment* env);

#endif // !LAMBDA_H
