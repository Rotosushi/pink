#ifndef CONDITIONAL_H
#define CONDITIONAL_H

#include "Location.h"
#include "Judgement.h"
#include "Judgement.h"
struct Environment;
struct Ast;

typedef struct Conditional
{
  Location    loc;
  struct Ast* cnd;
  struct Ast* fst;
  struct Ast* snd;
} Conditional;

void InitializeConditional(Conditional* cond, struct Ast* cnd, struct Ast* fst, struct Ast* snd, Location* loc);

void DestroyConditional(Conditional* cond);

void CloneConditional(Conditional* destination, Conditional* source);

char* ToStringConditional(Conditional* cond);

Judgement GetypeConditional(Conditional* node, struct Environment* env);

Judgement EvaluateConditional(Conditional* cond, struct Environment* env);

#endif // !CONDITIONAL_H
