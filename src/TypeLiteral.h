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

TypeJudgement GetypeTypeLiteral(TypeLiteral* tl, struct Environment* env);


#endif // !TYPELITERAL_H
