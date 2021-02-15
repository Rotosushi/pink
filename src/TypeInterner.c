#include <stdlib.h>

#include "ScalarType.h"
#include "ProcType.h"
#include "Type.h"
#include "TypeInterner.h"

TypeInterner* CreateTypeInterner()
{
  TypeInterner* result = (TypeInterner*)malloc(sizeof(TypeInterner));
  result->nilType     = NULL;
  result->integerType = NULL;
  result->booleanType = NULL;
  result->procTypes   = NULL;
  result->procTypesLength = 0;
  return result;
}

void DestroyTypeInterner(TypeInterner* Ity)
{

  for (int i = 0; i < Ity->procTypesLength; i++)
  {
    DestroyType(Ity->procTypes[i]);
  }
  free(Ity->procTypes);
  Ity->procTypesLength = 0;

  DestroyType(Ity->nilType);
  DestroyType(Ity->integerType);
  DestroyType(Ity->booleanType);
  free(Ity);
  Ity = NULL;
}

Type* GetNilType(TypeInterner* Ity)
{
  if (Ity->nilType == NULL)
  {
    Ity->nilType              = (Type*)malloc(sizeof(Type));
    Ity->nilType->kind        = T_SCALAR;
    Ity->nilType->scalar.kind = S_NIL;
  }

  return Ity->nilType;
}

Type* GetIntegerType(TypeInterner* Ity)
{
  if (Ity->integerType == NULL)
  {
    Ity->integerType              = (Type*)malloc(sizeof(Type));
    Ity->integerType->kind        = T_SCALAR;
    Ity->integerType->scalar.kind = S_INT;
  }


  return Ity->integerType;
}

Type* GetBooleanType(TypeInterner* Ity)
{
  if (Ity->booleanType == NULL)
  {
    Ity->booleanType              = (Type*)malloc(sizeof(Type));
    Ity->booleanType->kind        = T_SCALAR;
    Ity->booleanType->scalar.kind = S_BOOL;
  }

  return Ity->booleanType;
}

// this implementation seems worse and worse
// the more unique procedure types are defined.
// however it also seems like a complete solution,
// so this is absolutely a place for optimization.
Type* GetProcedureType(TypeInterner* Ity, Type* l, Type* r)
{
  if (Ity == NULL || l == NULL || r == NULL)
    return NULL;
  // search the buffer for a matching type,
  // otherwise insert the new type and
  // return it.

  Type* possibleType = (Type*)malloc(sizeof(Type));
  possibleType->kind = T_PROC;
  InitializeProcType(&(possibleType->proc), l, r);

  for (int i = 0; i < Ity->procTypesLength; i++)
  {
    Type* currentType = Ity->procTypes[i];
    if (EqualTypes(possibleType, currentType))
    {
      DestroyType(possibleType);
      return currentType;
    }
  }

  // insert the new type
  Ity->procTypesLength += 1;
  Ity->procTypes        = (Type**)realloc(Ity->procTypes, sizeof(Type*) * Ity->procTypesLength);
  Ity->procTypes[Ity->procTypesLength - 1] = possibleType;
  return possibleType;
}
