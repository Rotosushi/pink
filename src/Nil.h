#ifndef NIL_H
#define NIL_H

#include "TypeJudgement.h"

struct Environment;
struct Ast;
struct Object;

typedef struct Nil
{
  void* value;
} Nil;

void InitializeNil(Nil* nil);

void DestroyNil(Nil* nil);

void CloneNil(Nil* destination, Nil* source);

char* ToStringNil(Nil* nil);

TypeJudgement GetypeNil(struct Ast* node, struct Environment* env);

#endif // !NIL_H
