#include <stdlib.h>
#include <string.h>

#include "Utilities.hpp"
#include "Judgement.h"
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

Judgement GetypeNil(Nil* node, Environment* env)
{
  Judgement result;
  result.success = true;
  result.term    =  CreateAstType(GetNilType(env->interned_types), &(node->loc));
  return result;
}

Judgement AssignNil(Nil* dest, Nil* source)
{
  Judgement result;
  result.success = true;
  result.term    = NULL;
  return result;
}

Judgement EqualsNil(Nil* n1, Nil* n2)
{
  Judgement result;
  result.success = true;
  result.term    = NULL;
  return result;
}
