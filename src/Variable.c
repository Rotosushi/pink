#include <stdlib.h>
#include <string.h>

#include "Utilities.hpp"
#include "Ast.h"
#include "Environment.h"
#include "StringInterner.hpp"
#include "SymbolTable.h"
#include "Variable.h"

void InitializeVariable(Variable* variable, InternedString id, ScopeSet scope, Location* loc)
{
  variable->loc   = *loc;
  variable->id    = id;
  variable->scope = scope;
}

void DestroyVariable(Variable* var)
{
  return;
}

void CloneVariable(Variable* dest, Variable* source)
{
  dest->loc   = source->loc;
  dest->id    = source->id;
  dest->scope = source->scope;
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

  Judgement bound = LookupSymbol(env->symbols, var->id, var->scope);

  if (bound.success == true)
  {
    result = Getype(bound.term, env);
  }
  else
  {
    result = bound;
  }

  return result;
}

Judgement EvaluateVariable(Variable* var, struct Environment* env)
{
  Judgement result;

  result = LookupSymbol(env->symbols, var->id, var->scope);

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
