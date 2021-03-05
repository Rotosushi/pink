#ifndef BOOLEAN_H
#define BOOLEAN_H

#include <stdbool.h>

#include "Judgement.h"
#include "Location.h"
struct Ast;
struct Environment;

typedef struct Boolean
{
  Location loc;
  bool     value;
} Boolean;

void InitializeBoolean(Boolean* boolean, bool value, Location* loc);

void DestroyBoolean(Boolean* boolean);

void CloneBoolean(Boolean* destination, Boolean* source);

char* ToStringBoolean(Boolean* boolean);

Judgement GetypeBoolean(Boolean* node, struct Environment* env);

Judgement AssignBoolean(Boolean* dest, Boolean* source);

Judgement EqualsBoolean(Boolean* b1, Boolean* b2);

#endif // !BOOLEAN_H
