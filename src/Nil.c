#include <stdlib.h>
#include <string.h>

#include "Utilities.h"
#include "TypeJudgement.h"
#include "Ast.h"
#include "Environment.h"
#include "Nil.h"

void InitializeNil(Nil* nil)
{
  nil->value = NULL;
}

void DestroyNil(Nil* nil)
{
  return;
}

void CloneNil(Nil* dest, Nil* source)
{
  dest->value = source->value;
}

char* ToStringNil(Nil* nil)
{
  return dupnstr("nil", 3);
}

TypeJudgement GetypeNil(Ast* node, Environment* env)
{
  TypeJudgement result;
  result.success = true;
  result.type    = GetNilType(env->interned_types);
  return result;
}
