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
  return result;
}

void DestroyTypeInterner(TypeInterner* Ity)
{
  /*
  what could a procedure type be composed of?
  well, given that we always construct using the
  methods provided by the TypeInterner the only time we
  allocate a Proc type node is if it was being built to
  be inserted into the list. which it was given two valid
  left and right hand sides, which could be one of
  A) a pointer to a primitive type, Nil, Int, Bool.
  B) a pointer to another procedure type, itself stored within
     this list of procedure types here.
  */
  TLelem* cur = Ity->procTypes, *prv;
  while (cur != NULL)
  {
    prv = cur;
    cur = cur->next;
    free (prv->type);
    free (prv);
  }
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

// this implementation seems worse and worse
// the more unique procedure types are defined.
// however it also seems like a complete solution,
// so this is absolutely a place for optimization.
Type* GetProcedureType(TypeInterner* Ity, Ast* l, Ast* r)
{
  if (Ity == NULL || l == NULL || r == NULL)
    return NULL;


  // search the buffer for a matching type,
  // otherwise insert the new type and
  // return it.

  Type* possibleType = (Type*)malloc(sizeof(Type));
  possibleType->kind = T_PROC;
  possibleType->proc.lhs = l;
  possibleType->proc.rhs = r;


  TLelem *cur = Ity->procTypes, *prv = NULL;
  if (cur == NULL)
  {
    TLelem* newproc = (TLelem*)malloc(sizeof(TLelem));
    newproc->next  = NULL;
    newproc->type  = possibleType;
    Ity->procTypes = newproc;
    return possibleType;
  }
  else
  {
    while (cur != NULL)
    {
      prv = cur;
      Type* currentType = cur->type;
      if (EqualTypes(possibleType, currentType))
      {
        // huh, we don't want to call the regular
        // destructor, as that will recursively destroy
        // the scalar types at the bottom, which makes
        // all other references to that primitive type
        // bad pointers which break when dereferenced.
        // so we rest assured that the passed in pointers
        // are well formed, and that we do not own their memory.
        free(possibleType);
        return currentType;
      }
      cur = cur->next;
    }

    // insert the new type
    TLelem* newproc = (TLelem*)malloc(sizeof(TLelem));
    newproc->next  = NULL;
    newproc->type  = possibleType;
    prv->next      = newproc;
    return possibleType;
  }
}
