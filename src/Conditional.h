#ifndef CONDITIONAL_H
#define CONDITIONAL_H

struct Ast;

typedef struct Conditional
{
  struct Ast* cnd;
  struct Ast* fst;
  struct Ast* snd;
} Conditional;

void DestroyConditional(Conditional* cond);

Conditional* CloneConditional(Conditional* cond);

char* ToStringConditional(Conditional* cond);

#endif // !CONDITIONAL_H
