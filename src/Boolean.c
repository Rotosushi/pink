#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "Boolean.h"

void InitializeBoolean(Boolean* boolean, bool value)
{
  boolean->value = value;
}

void DestroyBoolean(Boolean* boolean)
{
  return;
}

void CloneBoolean(Boolean* destination, Boolean* source)
{
  destination->value = source->value;
}

// maybe we sould make this procedure
// allocate new memory for the
char* ToStringBoolean(Boolean* boolean)
{
  char*  result = ((b->value == false) ? "false" : "true");
  return result;
}
