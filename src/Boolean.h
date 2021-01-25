#ifndef BOOLEAN_H
#define BOOLEAN_H

#include <stdbool.h>

typedef struct Boolean
{
  bool value;
} Boolean;

void DestroyBoolean(Boolean* bool);

Boolean* CloneBoolean(Boolean* bool);

char* ToStringBoolean(Boolean* bool);

#endif // !BOOLEAN_H
