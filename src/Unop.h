#ifndef UNOP_H
#define UNOP_H

#include "StringInterner.h"

struct Ast;

typedef struct Unop
{
  InternedString op;
  struct Ast* rhs;
} Unop;

Unop* CreateUnop(InternedString op, struct Ast* rhs);

void DestroyUnop(Unop* unp);

Unop* CloneUnop(Unop* unp);

char* ToStringUnop(Unop* unp);

#endif // !UNOP_H
