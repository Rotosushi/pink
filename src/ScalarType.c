#include <stdlib.h>
#include <string.h>

#include "ScalarType.h"

ScalarType* CreateScalarType(ScalarKind kind)
{
  ScalarType* result = (ScalarType*)malloc(sizeof(ScalarType));
  result->kind       = kind;
  return result;
}

void DestroyScalarType(ScalarType* scalar)
{
  free(scalar);
  scalar = NULL;
}

ScalarType* CloneScalarType(ScalarType* scalar)
{
  ScalarType* result = (ScalarType*)malloc(sizeof(ScalarType));
  result->kind       = scalar->kind;
  return result;
}

char* ToStringScalarType(ScalarType* scalar)
{
  char* result;
  switch(scalar->kind)
  {
    case P_NIL:
      result = "Nil";
      break;
    case P_INT:
      result = "Int";
      break;
    case P_BOOL:
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
