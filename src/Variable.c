#include <stdlib.h>
#include <string.h>

#include "StringInterner.h"
#include "Variable.h"

void DestroyVariable(Variable* var)
{
  free(var);
}

Variable* CloneVariable(Variable* var)
{
  Variable* result = (Variable*)malloc(sizeof(Variable));
  result->id = var->id;
  return result;
}

char* ToStringVariable(Variable* var)
{
  return (char*)var->id;
}
