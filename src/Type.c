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
    case T_ATOM:
      DestroyScalarType(type->atom);
      break;
    case T_PROC:
      DestroyProcType(type->proc);
      break;
    default:
      FatalError("Bad Type Kind", __FILE__, __LINE__);
      break;
  }
  free(type);
}

Type* CloneType(Type* type)
{
  Type* result = (Type*)malloc(sizeof(Type));
  result->kind = type->kind;
  switch (result->kind)
  {
    case T_ATOM:
      result->atom = CloneScalarType(type->atom);
      break;
    case T_PROC:
      result->proc = CloneProcType(type->proc);
      break;
    default:
      FatalError("Bad Type Kind", __FILE__, __LINE__);
      break;
  }
  return result;
}

char* ToStringType(Type* type)
{
  char* result;
  switch (type->kind)
  {
    case T_ATOM:
      result = ToStringScalarType(type->atom);
      break;
    case T_PROC:
      result = ToStringProcType(type->proc);
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
    case T_ATOM:
      if (t2->kind == T_ATOM)
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
