#include <stdlib.h>
#include <string.h>

#include "Utilities.h"
#include "Ast.h"
#include "Environment.h"
#include "StringInterner.h"
#include "SymbolTable.h"
#include "Variable.h"

void InitializeVariable(Variable* variable, InternedString id)
{
  variable->id = id;
}

void DestroyVariable(Variable* var)
{
  free(var);
}

void CloneVariable(Variable* dest, Variable* source)
{
  dest->id = source->id;
}

char* ToStringVariable(Variable* var)
{
  return dupnstr(var->id, strlen(var->id));
}

// the type of a variable is the type
// of whatever the variable is currently
// bound to.
TypeJudgement GetypeVariable(Ast* node, Environment* env)
{
  TypeJudgement result;

  Variable* var = &(node->var);

  Ast* bound_term = lookup(env->outer_scope, var->id);

  if (bound_term)
  {
    result = Getype(bound_term, env);
  }
  else
  {
    result.success   = false;
    result.error.dsc = "variable not defined within scope";
    result.error.loc = node->loc;
  }

  return result;
}
