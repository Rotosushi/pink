#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Integer.h"

void DestroyInteger(Integer* i)
{
  free(i);
  i = NULL;
}

Integer* CloneInteger(Integer* i)
{
  Integer* result = (Integer*)malloc(sizeof(Integer));
  result->value   = i->value;
  return result;
}

char* ToStringInteger(Integer* i)
{
  char* result = (char*)calloc((sizeof(int) * 8) + 1, sizeof(char));
  sprintf(result, "%d", i->value);
  return result;
}
