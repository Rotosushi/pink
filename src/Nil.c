#include <stdlib.h>
#include <string.h>

#include "Nil.h"

Nil* CreateNil()
{
  Nil* result = (Nil*)malloc(sizeof(Nil));
  result->value = NULL;
  return result;
}

void DestroyNil(Nil* nil)
{
  free(nil);
  nil = NULL;
}

Nil* CloneNil(Nil* nil)
{
  return nil;
}

char* ToStringNil(Nil* nil)
{
  return "nil";
}
