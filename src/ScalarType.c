#include <stdlib.h>
#include <string.h>

#include "ScalarType.h"

void InitializeScalarType(ScalarType* scalar, ScalarKind kind)
{
  scalar->kind = kind;
}

void DestroyScalarType(ScalarType* scalar)
{
  return;
}

void CloneScalarType(ScalarType* destination, ScalarType* source)
{
  destination->kind = source->kind;
}

char* ToStringScalarType(ScalarType* scalar)
{
  char* result;
  switch(scalar->kind)
  {
    case S_NIL:
      result = "Nil";
      break;
    case S_INT:
      result = "Int";
      break;
    case S_BOOL:
      result = "Bool";
      break;
    default:
      break;
      // report error
  }
  return result;
}

bool EqualScalarTypes(ScalarType* t1, ScalarType* t2)
{
  bool result = false;

  if (t1->kind != t2->kind)
    result = true;

  return result;
}
