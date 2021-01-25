#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "AtomType.h"
#include "ProcType.h"
#include "Type.h"


void DestroyType(Type* type)
{
  switch (type->kind)
  {
    case T_ATOM:
      DestroyAtomType(type->atom);
      break;
    case T_PROC:
      DestroyProcType(type->proc);
      break;
    default:
      // report error
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
      result->atom = CloneAtomType(type->atom);
      break;
    case T_PROC:
      result->proc = CloneProcType(type->proc);
      break;
    default:
      // report error
  }
  return result;
}

char* ToStringType(Type* type)
{
  char* result;
  switch (type->kind)
  {
    case T_ATOM:
      result = ToStringAtomType(type->atom);
      break;
    case T_PROC:
      result = ToStringProcType(type->proc);
      break;
    default:
      // report error
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
        result = EqualAtomTypes((AtomType*)t1, (AtomType*)t2);
      break;

    case T_PROC:
      if (t2->kind == T_PROC)
        result = EqualProcTypes((ProcType*)t1, (ProcType*)t2);
      break;

    default:
      // fatal error: unknown t1 type kind.
  }
  return result;
}
