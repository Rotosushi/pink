#include <stdlib.h>
#include <string.h>

#include "Utilities.h"
#include "TypeJudgement.h"
#include "Ast.h"
#include "Environment.h"
#include "Nil.h"

void InitializeNil(Nil* nil, Location* loc)
{
  nil->loc   = *loc;
  nil->value = NULL;
}

void DestroyNil(Nil* nil)
{
  return;
}

void CloneNil(Nil* dest, Nil* source)
{
  dest->loc   = source->loc;
  dest->value = source->value;
}

char* ToStringNil(Nil* nil)
{
  return dupnstr("nil", 3);
}

TypeJudgement GetypeNil(Nil* node, Environment* env)
{
  TypeJudgement result;
  result.success = true;
  result.type    = GetNilType(env->interned_types);
  return result;
}
