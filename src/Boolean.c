#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "Boolean.h"

void DestroyBoolean(Boolean* bol)
{
  free(bol);
  bol = NULL;
}

Boolean* CloneBoolean(Boolean* bol)
{
  Boolean* result = (Boolean*)malloc(sizeof(Boolean));
  result->value   = bol->value;
  return result;
}

char* ToStringBoolean(Boolean* bol)
{
  char* result = bol->value == false ? "false" : "true";
  return result;
}
