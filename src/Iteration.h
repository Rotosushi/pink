#ifndef ITERATION_H
#define ITERATION_H

struct Ast;

typedef struct Iteration
{
  struct Ast* cnd;
  struct Ast* bdy;
} Iteration;

void DestroyIteration(Iteration* itr);

Iteration* CloneIteration(Iteration* itr);

char* ToStringIteration(Iteration* itr);

#endif // !ITERATION_H
