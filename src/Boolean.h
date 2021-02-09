#ifndef BOOLEAN_H
#define BOOLEAN_H

#include <stdbool.h>

typedef struct Boolean
{
  bool value;
} Boolean;

void InitializeBoolean(Boolean* boolean, bool value);

void DestroyBoolean(Boolean* boolean);

void CloneBoolean(Boolean* destination, Boolean* source);

char* ToStringBoolean(Boolean* boolean);

#endif // !BOOLEAN_H
