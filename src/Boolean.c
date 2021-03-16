#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "Utilities.h"
#include "Ast.h"
#include "Environment.h"
#include "Boolean.h"

void InitializeBoolean(Boolean* boolean, bool value, Location* loc)
{
  boolean->loc   = *loc;
  boolean->value = value;
}

void DestroyBoolean(Boolean* boolean)
{
  return;
}

void CloneBoolean(Boolean* destination, Boolean* source)
{
  destination->loc   = source->loc;
  destination->value = source->value;
}

// you might ask, why are you returning a copy
// of data you already have represented?
// and that's a valid question, as this
// is on it's face, inefficient.
// however, we need to manage our own memory
// here, so, to make the implementation of
// all of the other ToString* procedures
// simpler, we return a copy, this means that
// we can uniformly make the assumption that
// ToString* gives you a buffer of memory
// that it is the callers resposibility to
// manage. this allows us to call free() on
// all intermediary buffers and be sure that
// we don't leak memory, nor will we call
// free on memory held within the interner,
// which would happen with identifiers,
// nor would we call free on static strings,
// which would happen with boolean literals
// and nil literals. (which does cause a segfault
// you are correct.)
char* ToStringBoolean(Boolean* boolean)
{
  char*  result = ((boolean->value == false) ? dupnstr("false", 5) : dupnstr("true", 4));
  return result;
}

Judgement GetypeBoolean(Boolean* node, Environment* env)
{
  Judgement result;
  result.success = true;
  result.term    = GetBooleanType(env->interned_types, &(node->loc));
  return result;
}

Judgement AssignBoolean(Boolean* dest, Boolean* source)
{
  Judgement result;
  result.success = true;
  result.term    = NULL;
  dest->value    = source->value;
  return result;
}

Judgement EqualsBoolean(Boolean* b1, Boolean* b2)
{
  Judgement result;
  result.success = true;
  result.term    = CreateAstBoolean((b1->value == b2->value), &(b1->loc));
  return result;
}
