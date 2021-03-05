#include <stdlib.h>
#include <string.h>

#include "Utilities.h"
#include "Ast.h"
#include "Environment.h"
#include "StringInterner.h"
#include "SymbolTable.h"
#include "Variable.h"

void InitializeVariable(Variable* variable, InternedString id, Location* loc)
{
  variable->loc = *loc;
  variable->id  = id;
}

void DestroyVariable(Variable* var)
{
  return;
}

void CloneVariable(Variable* dest, Variable* source)
{
  dest->loc = source->loc;
  dest->id  = source->id;
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
char* ToStringVariable(Variable* var)
{
  return dupnstr(var->id, strlen(var->id));
}

// the type of a variable is the type
// of whatever the variable is currently
// bound to.
Judgement GetypeVariable(Variable* var, Environment* env)
{
  Judgement result;

  Ast* bound_term = lookup(env->outer_scope, var->id);

  if (bound_term)
  {
    result = Getype(bound_term, env);
  }
  else
  {
    result.success   = false;
    result.error.dsc = "variable not defined within scope";
    result.error.loc = var->loc;
  }

  return result;
}

Judgement EvaluateVariable(Variable* var, struct Environment* env)
{
  Judgement result;

  Ast* bound_term = lookup(env->outer_scope, var->id);

  if (bound_term != NULL)
  {
    result.success = true;
    result.term    = bound_term;
  }
  else
  {
    result.success = false;
    result.error.dsc = "variable not bound within environment";
    result.error.loc = var->loc;
  }


  return result;
}
