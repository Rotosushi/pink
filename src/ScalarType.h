#ifndef ScalarType_H
#define ScalarType_H

#include <stdbool.h>

// or scalar type maybe?
typedef enum ScalarKind
{
  S_NIL,
  S_INT,
  S_BOOL,
} ScalarKind;

typedef struct ScalarType
{
  ScalarKind kind;
} ScalarType;

void InitializeScalarType(ScalarType* scalar, ScalarKind kind);

void DestroyScalarType(ScalarType* scalar);

void CloneScalarType(ScalarType* destination, ScalarType* source);

char* ToStringScalarType(ScalarType* source);

bool EqualScalarTypes(ScalarType* t1, ScalarType* t2);

#endif // !ScalarType_H
