#ifndef BIND_H
#define BIND_H

#include "Location.h"
#include "StringInterner.h"

struct Ast;

typedef struct Bind
{
  InternedString id;
  struct Ast*    rhs;
} Bind;

void InitializeBind(Bind* bind, InternedString id, struct Ast* right);

void DestroyBind(Bind* bnd);

void CloneBind(Bind* destination, Bind* source);

char* ToStringBind(Bind* bnd);

TypeJudgement GetypeBind(struct Ast* bnd, struct Environment* env);

#endif // !BIND_H
