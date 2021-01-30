#ifndef BOOLEAN_H
#define BOOLEAN_H

#include <stdbool.h>

typedef struct Boolean
{
  bool value;
} Boolean;

Boolean* CreateBoolean(bool b);

void DestroyBoolean(Boolean* b);

Boolean* CloneBoolean(Boolean* b);

char* ToStringBoolean(Boolean* b);

#endif // !BOOLEAN_H
