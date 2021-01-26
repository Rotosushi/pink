#ifndef ATOMTYPE_H
#define ATOMTYPE_H

#include <stdbool.h>

// or scalar type maybe?
typedef enum AtomKind
{
  P_NIL,
  P_INT,
  P_BOOL,
} AtomKind;

typedef struct AtomType
{
  AtomKind kind;
} AtomType;

void DestroyAtomType(AtomType* type);

AtomType* CloneAtomType(AtomType* type);

char* ToStringAtomType(AtomType* type);

bool EqualAtomTypes(AtomType* t1, AtomType* t2);

#endif // !ATOMTYPE_H
