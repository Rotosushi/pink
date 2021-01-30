#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Integer.h"

Integer* CreateInteger(int i)
{
  Integer* result = (Integer*)malloc(sizeof(Integer));
  result->value   = i
  return result;
}

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
  // allocate enough to fit the largest possible
  // int result possible, so we never generate
  // code which writes past the end of the array.
  char* result = (char*)calloc((sizeof(int) * 8) + 1, sizeof(char));
  sprintf(result, "%d", i->value);
  return result;
}
