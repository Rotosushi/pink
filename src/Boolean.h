#ifndef BOOLEAN_H
#define BOOLEAN_H

#include <stdbool.h>

#include "TypeJudgement.h"
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

TypeJudgement GetypeBoolean(Boolean* node, struct Environment* env);

#endif // !BOOLEAN_H
