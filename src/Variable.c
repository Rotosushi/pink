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

  Ast* bound_term = Lookup(env->symbols, var->id);

  if (bound_term)
  {
    result = Getype(bound_term, env);
  }
  else
  {
    char* c0 = "Variable [";
    char* c1 = "] not bound within environment";
    char* i0 = appendstr(c0, (char*)var->id);
    result.success = false;
    result.error.dsc = appendstr(i0, c1);
    result.error.loc = var->loc;
    free(i0);
  }

  return result;
}

Judgement EvaluateVariable(Variable* var, struct Environment* env)
{
  Judgement result;

  Ast* bound_term = Lookup(env->symbols, var->id);

  if (bound_term != NULL)
  {
    result.success = true;
    result.term    = bound_term;
  }
  else
  {
    char* c0 = "Variable [";
    char* c1 = "] not bound within environment";
    char* i0 = appendstr(c0, (char*)var->id);
    result.success = false;
    result.error.dsc = appendstr(i0, c1);
    result.error.loc = var->loc;
    free(i0);
  }

  return result;
}

bool AppearsFreeVariable(Variable* var, InternedString id)
{
  // the only real choice we make during traversal
  if (var->id == id)
    return true;
  else
    return false;
}

void RenameBindingVariable(Variable* var, InternedString target, InternedString replacement)
{
  if (var->id == target)
    var->id = replacement;
}

void SubstituteVariable(Variable* var, Ast** target, InternedString id, Ast* value, struct Environment* env)
{
  if (var->id == id)
    (*target) = value;
}
