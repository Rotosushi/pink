#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "Utilities.h"
#include "Ast.h"
#include "Environment.h"
#include "Boolean.h"

void InitializeBoolean(Boolean* boolean, bool value)
{
  boolean->value = value;
}

void DestroyBoolean(Boolean* boolean)
{
  return;
}

void CloneBoolean(Boolean* destination, Boolean* source)
{
  destination->value = source->value;
}

// maybe we sould make this procedure
// allocate new memory for the text each
// time so the consuming code can uniformly
// perform deallocations without having to
// consider the actual types of things.
char* ToStringBoolean(Boolean* boolean)
{
  char*  result = ((boolean->value == false) ? dupnstr("false", 5) : dupnstr("true", 4));
  return result;
}

TypeJudgement GetypeBoolean(Ast* node, Environment* env)
{
  TypeJudgement result;
  result.success = true;
  result.type    = GetBooleanType(env->interned_types);
  return result;
}
