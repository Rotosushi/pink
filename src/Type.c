#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "PinkError.h"
#include "ScalarType.h"
#include "ProcType.h"
#include "Type.h"


void DestroyType(Type* type)
{
  switch (type->kind)
  {
    case T_SCALAR:
      DestroyScalarType(&(type->scalar));
      break;
    case T_PROC:
      DestroyProcType(&(type->proc));
      break;
    default:
      FatalError("Bad Type Kind", __FILE__, __LINE__);
      break;
  }
}

void CloneType(Type* destination, Type* source)
{
  destination->kind = source->kind;
  switch (source->kind)
  {
    case T_SCALAR:
      CloneScalarType(&(destination->scalar), &(source->scalar));
      break;
    case T_PROC:
      CloneProcType(&(destination->proc), &(source->proc));
      break;
    default:
      FatalError("Bad Type Kind", __FILE__, __LINE__);
      break;
  }
}

char* ToStringType(Type* type)
{
  char* result;
  switch (type->kind)
  {
    case T_SCALAR:
      result = ToStringScalarType(&(type->scalar));
      break;
    case T_PROC:
      result = ToStringProcType(&(type->proc));
      break;
    default:
      FatalError("Bad Type Kind", __FILE__, __LINE__);
      break;
  }
  return result;
}

bool EqualTypes(Type* t1, Type* t2)
{
  bool result = false;
  switch (t1->kind)
  {
    case T_SCALAR:
      if (t2->kind == T_SCALAR)
        result = EqualScalarTypes((ScalarType*)t1, (ScalarType*)t2);
      break;

    case T_PROC:
      if (t2->kind == T_PROC)
        result = EqualProcTypes((ProcType*)t1, (ProcType*)t2);
      break;

    default:
      FatalError("Bad Type Kind", __FILE__, __LINE__);
      break;
  }
  return result;
}
