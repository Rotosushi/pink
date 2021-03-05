#ifndef INTEGER_H
#define INTEGER_H

#include "Judgement.h"
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

Judgement GetypeInteger(Integer* node, struct Environment* env);

Judgement AssignInteger(Integer* dest, Integer* source);

Judgement EqualsInteger(Integer* i1, Integer* i2);

#endif // !INTEGER_H
