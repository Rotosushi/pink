#ifndef TYPE_H
#define TYPE_H
#include <stdbool.h>

#include "ScalarType.h"
#include "ProcType.h"

typedef enum TypeKind
{
  T_SCALAR,
  T_PROC,
} TypeKind;

typedef struct Type
{
  TypeKind kind;
  union {
    ScalarType scalar;
    ProcType   proc;
    // RefType ref;
  };
} Type;

void DestroyType(Type* type);

void CloneType(Type** destination, Type* source);

char* ToStringType(Type* type);

bool EqualTypes(Type* t1, Type* t2);

#endif // !TYPE_H
