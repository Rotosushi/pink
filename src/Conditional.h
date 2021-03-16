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

bool AppearsFreeConditional(Conditional* cond, InternedString id);

void RenameBindingConditional(Conditional* cond, InternedString target, InternedString replacement);

void SubstituteConditional(Conditional* cond, struct Ast** target, InternedString id, struct Ast* value, struct Environment* env);
#endif // !CONDITIONAL_H
