#ifndef BIND_H
#define BIND_H

#include "Location.h"
#include "StringInterner.h"

struct Ast;

typedef struct Bind
{
  Location       loc;
  InternedString id;
  struct Ast*    rhs;
} Bind;

void InitializeBind(Bind* bind, InternedString id, struct Ast* right, Location* loc);

void DestroyBind(Bind* bnd);

void CloneBind(Bind* destination, Bind* source);

char* ToStringBind(Bind* bnd);

TypeJudgement GetypeBind(Bind* bnd, struct Environment* env);

#endif // !BIND_H
