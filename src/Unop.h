#ifndef UNOP_H
#define UNOP_H

#include "StringInterner.h"

struct Ast;

typedef struct Unop
{
  InternedString op;
  struct Ast* rhs;
} Unop;

void DestroyUnop(Unop* unp);

Unop* CloneUnop(Unop* unp);

char* ToStringUnop(Unop8 unp);

#endif // !UNOP_H
