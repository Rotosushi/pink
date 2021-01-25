#include <stdlib.h>
#include <string.h>

#include "Nil.h"

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
