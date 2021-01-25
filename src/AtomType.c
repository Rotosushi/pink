#include <stdlib.h>
#include <string.h>

#include "AtomType.h"

void DestroyAtomType(AtomType* atom)
{
  free(atom);
  atom = NULL;
}

AtomType* CloneAtomType(AtomType* atom)
{
  AtomType* result = (AtomType*)malloc(sizeof(AtomType));
  result->kind = atom->kind;
  return result;
}

char* ToStringAtomType(AtomType* atom)
{
  char* result;
  switch(atom->kind)
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
      // resport error
  }
  return result;
}

bool EqualAtomTypes(AtomType* t1, AtomType* t2)
{
  bool result = false;

  if (t1->kind != t2->kind)
    result = true;
    
  return result;
}
