#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "Boolean.h"

void DestroyBoolean(Boolean* b)
{
  free(b);
  b = NULL;
}

Boolean* CloneBoolean(Boolean* b)
{
  Boolean* result = (Boolean*)malloc(sizeof(Boolean));
  result->value   = b->value;
  return result;
}

char* ToStringBoolean(Boolean* b)
{
  char* result = ((b->value == false) ? "false" : "true");
  return result;
}
