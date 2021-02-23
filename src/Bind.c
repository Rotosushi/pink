#include <stdlib.h>
#include <string.h>

#include "Ast.h"
#include "SymbolTable.h"
#include "Environment.h"
#include "StringInterner.h"
#include "Utilities.h"
#include "Bind.h"

void InitializeBind(Bind* bind, InternedString id, struct Ast* right, Location* loc)
{
  bind->loc = *loc;
  bind->id  = id;
  bind->rhs = right;
}

void DestroyBind(Bind* bind)
{
  DestroyAst(bind->rhs);
}

void CloneBind(Bind* destination, Bind* source)
{
  destination->loc = source->loc;
  destination->id  = source->id;
  CloneAst(&(destination->rhs), source->rhs);
}

char* ToStringBind(Bind* bnd)
{
  char *result, *clneq, *right, *ctx;
  clneq = " := "; // 4 + 1 = 5
  right = ToStringAst(bnd->rhs);

  // string interning is so, so, so clever. :)
  int sz = strlen((char*)bnd->id) + strlen(right) + 5;
  result = (char*)calloc(sz, sizeof(char));

  strkat(result, (char*)bnd->id, &ctx);
  strkat(result, clneq,          &ctx);
  strkat(result, right,          &ctx);
  free(right);
  return result;
}


TypeJudgement GetypeBind(Bind* bnd, Environment* env)
{
  TypeJudgement result;

  Ast* bound_term = lookup_in_local_scope(env->outer_scope, bnd->id);

  if (bound_term == NULL)
  {
    TypeJudgement rhsjdgmt = Getype(bnd->rhs, env);
    // when we add a sequence (';') grapheme, we need to
    // bind to the type of the rhs to properly
    // typecheck terms in our grammar.
    // because we handle the sequence via recursion.
    // sequence has essentially the same storage pattern as
    // application, assignment, bind, and iteration.
    result = rhsjdgmt;
  }
  else
  {
    result.success   = false;
    result.error.dsc = "Id is already bound in scope";
    result.error.loc = bnd->loc;
  }
  return result;
}
