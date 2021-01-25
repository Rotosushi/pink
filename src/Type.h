#ifndef TYPE_H
#define TYPE_H
#include <stdbool.h>


#include "AtomType.h"
#include "ProcType.h"

typedef enum TypeKind
{
  T_ATOM,
  T_PROC,
} TypeKind;

typedef struct Type
{
  TypeKind kind;
  union {
    AtomType *atom;
    ProcType *proc;
    // RefType ref;
  };
} Type;

void DestroyType(Type* type);

Type* CloneType(Type* type);

char* ToStringType(Type* type);

bool EqualTypes(Type* t1, Type* t2);

#endif // !TYPE_H
