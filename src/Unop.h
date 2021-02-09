#ifndef UNOP_H
#define UNOP_H

#include "StringInterner.h"

struct Environment;
struct Ast;

typedef struct Unop
{
  InternedString op;
  struct Ast* rhs;
} Unop;

void InitializeUnop(Unop* unop, InternedString op, struct Ast* rhs);

void DestroyUnop(Unop* unp);

void CloneUnop(Unop* destination, Unop* source);

char* ToStringUnop(Unop* unp);

TypeJudgement GetypeUnop(Unop* uop, struct Environment* env);

#endif // !UNOP_H
