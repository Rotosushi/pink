#ifndef UNOP_H
#define UNOP_H

#include "StringInterner.h"
#include "Judgement.h"
#include "Judgement.h"
#include "Location.h"
struct Environment;
struct Ast;

typedef struct Unop
{
  Location       loc;
  InternedString op;
  struct Ast*    rhs;
} Unop;

void InitializeUnop(Unop* unop, InternedString op, struct Ast* rhs, Location* loc);

void DestroyUnop(Unop* unp);

void CloneUnop(Unop* destination, Unop* source);

char* ToStringUnop(Unop* unp);

Judgement GetypeUnop(Unop* uop, struct Environment* env);

Judgement EvaluateUnop(Unop* uop, struct Environment* env);

#endif // !UNOP_H
