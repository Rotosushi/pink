#ifndef BIND_H
#define BIND_H

#include "StringInterner.h"

struct Ast;

typedef struct Bind
{
  InternedString id;
  struct Ast*    rhs;
} Bind;

void DestroyBind(Bind* bnd);

Bind* CloneBind(Bind* bnd);

char* ToStringBind(Bind* bnd);

#endif // !BIND_H
