#ifndef ITERATION_H
#define ITERATION_H

struct Environment;
struct Ast;

typedef struct Iteration
{
  struct Ast* cnd;
  struct Ast* bdy;
} Iteration;

void InitializeIteration(Iteration* itr, struct Ast* cnd, struct Ast* bdy);

void DestroyIteration(Iteration* itr);

void CloneIteration(Iteration* destination, Iteration* source);

char* ToStringIteration(Iteration* itr);

TypeJudgement GetypeIteration(Iteration* itr, struct Environment* env);

#endif // !ITERATION_H
