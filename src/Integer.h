#ifndef INTEGER_H
#define INTEGER_H

#include "TypeJudgement.h"
#include "Location.h"
struct Ast;
struct Environment;

typedef struct Integer
{
  Location loc;
  int      value;
} Integer;

void InitializeInteger(Integer* integer, int value, Location* loc);

void DestroyInteger(Integer* integer);

void CloneInteger(Integer* destination, Integer* source);

char* ToStringInteger(Integer* integer);

TypeJudgement GetypeInteger(Integer* node, struct Environment* env);

#endif // !INTEGER_H
