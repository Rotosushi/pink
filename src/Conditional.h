#ifndef CONDITIONAL_H
#define CONDITIONAL_H


struct Environment;
struct Ast;

typedef struct Conditional
{
  struct Ast* cnd;
  struct Ast* fst;
  struct Ast* snd;
} Conditional;

void InitializeConditional(Conditional* cond, struct Ast* cnd, struct Ast* fst, struct Ast* snd);

void DestroyConditional(Conditional* cond);

void CloneConditional(Conditional* destination, Conditional* source);

char* ToStringConditional(Conditional* cond);

TypeJudgement GetypeConditional(struct Ast* node, struct Environment* env);

#endif // !CONDITIONAL_H
