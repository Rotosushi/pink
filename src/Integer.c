#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Ast.h"
#include "Location.h"
#include "Judgement.h"
#include "TypeInterner.h"
#include "Environment.h"
#include "Integer.h"

void InitializeInteger(Integer* integer, int i, Location* loc)
{
  integer->loc   = *loc;
  integer->value = i;
}

void DestroyInteger(Integer* i)
{
  return;
}

void CloneInteger(Integer* destination, Integer* source)
{
  destination->loc   = source->loc;
  destination->value = source->value;
}

char* ToStringInteger(Integer* i)
{
  // allocate enough to fit the largest possible
  // int result possible, so we never generate
  // code which writes past the end of the array.
  char* result = (char*)calloc((sizeof(int) * 8) + 1, sizeof(char));
  sprintf(result, "%d", i->value); // convert integer to string.
  return result;
}

Judgement GetypeInteger(Integer* node, Environment* env)
{
  Judgement result;
  result.success = true;
  result.term    = GetIntegerType(env->interned_types, &(node->loc));
  return result;
}

Judgement AssignInteger(Integer* dest, Integer* source)
{
  Judgement result;
  result.success = true;
  result.term    = NULL;
  dest->value    = source->value;
  return result;
}

Judgement EqualsInteger(Integer* i1, Integer* i2)
{
  Judgement result;
  result.success = true;
  result.term    = CreateAstBoolean((i1->value == i2->value), &(i1->loc));
  return result;
}
