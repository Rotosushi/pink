#ifndef BOOLEAN_H
#define BOOLEAN_H

#include <stdbool.h>

#include "TypeJudgement.h"

struct Ast;
struct Environment;

typedef struct Boolean
{
  bool value;
} Boolean;

void InitializeBoolean(Boolean* boolean, bool value);

void DestroyBoolean(Boolean* boolean);

void CloneBoolean(Boolean* destination, Boolean* source);

char* ToStringBoolean(Boolean* boolean);

TypeJudgement GetypeBoolean(struct Ast* node, struct Environment* env);

#endif // !BOOLEAN_H
