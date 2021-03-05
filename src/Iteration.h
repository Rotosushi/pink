#ifndef ITERATION_H
#define ITERATION_H

#include "Location.h"
struct Environment;
struct Ast;

typedef struct Iteration
{
  Location    loc;
  struct Ast* cnd;
  struct Ast* bdy;
} Iteration;

void InitializeIteration(Iteration* itr, struct Ast* cnd, struct Ast* bdy, Location* loc);

void DestroyIteration(Iteration* itr);

void CloneIteration(Iteration* destination, Iteration* source);

char* ToStringIteration(Iteration* itr);

Judgement GetypeIteration(Iteration* itr, struct Environment* env);

Judgement EvaluateIteration(Iteration* itr, struct Environment* env);

#endif // !ITERATION_H
