#ifndef NIL_H
#define NIL_H

#include "TypeJudgement.h"
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

TypeJudgement GetypeNil(Nil* node, struct Environment* env);

#endif // !NIL_H
