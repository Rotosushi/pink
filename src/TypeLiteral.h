#ifndef TYPELITERAL_H
#define TYPELITERAL_H

#include "Location.h"
#include "Type.h"
struct Environment;

typedef struct TypeLiteral
{
  Location loc;
  Type*    literal;
} TypeLiteral;

void InitializeTypeLiteral(TypeLiteral* tl, Type* literal, Location* loc);

void DestroyTypeLiteral(TypeLiteral* tl);

void CloneTypeLiteral(TypeLiteral* dest, TypeLiteral* source);

char* ToStringTypeLiteral(TypeLiteral* tl);

Judgement GetypeTypeLiteral(TypeLiteral* tl, struct Environment* env);

Judgement AssignTypeLiteral(TypeLiteral* dest, TypeLiteral* source);

Judgement EqualsTypeLiteral(TypeLiteral* t1, TypeLiteral* t2);

#endif // !TYPELITERAL_H
