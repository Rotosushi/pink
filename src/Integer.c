#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Ast.h"
#include "TypeJudgement.h"
#include "TypeInterner.h"
#include "Environment.h"
#include "Integer.h"

void InitializeInteger(Integer* integer, int i)
{
  integer->value = i;
}

void DestroyInteger(Integer* i)
{
  return;
}

void CloneInteger(Integer* destination, Integer* source)
{
  destination->value = source->value;
}

char* ToStringInteger(Integer* i)
{
  // allocate enough to fit the largest possible
  // int result possible, so we never generate
  // code which writes past the end of the array.
  char* result = (char*)calloc((sizeof(int) * 8) + 1, sizeof(char));
  sprintf(result, "%d", i->value);
  return result;
}

TypeJudgement GetypeInteger(Ast* node, Environment* env)
{
  TypeJudgement result;
  result.success = true;
  result.type    = GetIntegerType(env->interned_types);
  return result;
}
