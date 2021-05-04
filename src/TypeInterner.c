#include <stdlib.h>

#include "ScalarType.h"
#include "ProcType.h"
#include "Ast.h"
#include "Type.h"
#include "TypeInterner.h"

TypeInterner* CreateTypeInterner()
{
  TypeInterner* result = (TypeInterner*)malloc(sizeof(TypeInterner));
  result->nilType     = NULL;
  result->integerType = NULL;
  result->booleanType = NULL;
  result->procTypes   = NULL;
  result->cntProcTypes = 0;
  return result;
}

void DestroyTypeInterner(TypeInterner* Ity)
{
  free (Ity->procTypes);
  free (Ity->nilType);
  free (Ity->integerType);
  free (Ity->booleanType);
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



// we optimized it out of being a linked list.
// now it's an array of cells of Type, each with
// pointers to the scalar types, or another proctype
// cell.
Type* GetProcedureType(TypeInterner* Ity, Type* l, Type* r)
{
  if (Ity == NULL || l == NULL || r == NULL)
    return NULL;

  // look for a matching type within the already
  // allocated proctypes, such that we can maintain
  // the 'singleton' constraint upon Type*'s
  for (int i = 0; i < Ity->cntProcTypes; i++)
  {
    // with structured bindings, could this be
    // optimized to a single lookup, or, I guess
    // the whole array would be pulled into cache
    // by the first lookup? (or the first few cells at least)
    // (I also suppose if that were the case, data-analysis accross the two
    // instructions would also reveal that optimization?)
    Type* heldlhs = Ity->procTypes[i].proc.lhs;
    Type* heldrhs = Ity->procTypes[i].proc.rhs;

    // we don't use EqualTypes here to avoid doing any
    // recursive typechecking
    if (heldlhs == l && heldrhs == r)
    {
      // return the address of the cell within the array holding the (Type)
      // i.e. a Type*
      return &(Ity->procTypes[i]);
    }
  }

  // we didn't find a procType with the correct lhs and rhs.
  // so we build one and insert into the buffer.
  Ity->cntProcTypes += 1;
  Ity->procTypes     = (Type*)realloc(Ity->procTypes, sizeof(Type) * Ity->cntProcTypes);
  Type* newProcType = &(Ity->procTypes[Ity->cntProcTypes - 1]);
  newProcType->kind = T_PROC;
  InitializeProcType(&newProcType->proc, l, r);
  return newProcType;
}
