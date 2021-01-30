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

ScalarType* CreateScalarType(ScalarKind kind);

void DestroyScalarType(ScalarType* type);

ScalarType* CloneScalarType(ScalarType* type);

char* ToStringScalarType(ScalarType* type);

bool EqualScalarTypes(ScalarType* t1, ScalarType* t2);

#endif // !ScalarType_H
