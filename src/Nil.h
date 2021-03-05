#ifndef NIL_H
#define NIL_H

#include "Judgement.h"
#include "Location.h"
struct Environment;
struct Ast;
struct Object;

typedef struct Nil
{
  Location loc;
  void*    value;
} Nil;

void InitializeNil(Nil* nil, Location* loc);

void DestroyNil(Nil* nil);

void CloneNil(Nil* destination, Nil* source);

char* ToStringNil(Nil* nil);

Judgement GetypeNil(Nil* node, struct Environment* env);

Judgement AssignNil(Nil* dest, Nil* source);

Judgement EqualsNil(Nil* n1, Nil* n2);

#endif // !NIL_H
