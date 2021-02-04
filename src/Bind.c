#include <stdlib.h>
#include <string.h>

#include "Ast.h"
#include "StringInterner.h"
#include "Bind.h"

Bind* CreateBind(InternedString id, struct Ast* right)
{
  Bind* result = (Bind*)malloc(sizeof(Bind));
  result->id   = id;
  result->rhs  = right;
  return result;
}

void DestroyBind(Bind* bnd)
{
  DestroyAst(bnd->rhs);
  free(bnd);
  bnd = NULL;
}

Bind* CloneBind(Bind* bnd)
{
  Bind* result = (Bind*)malloc(sizeof(Bind));
  result->id   = bnd->id;
  result->rhs  = CloneAst(bnd->rhs);
  return result;
}

char* ToStringBind(Bind* bnd)
{
  char *result, *clneq, *right;
  clneq = " := "; // 4 + 1 = 5
  right = ToStringAst(bnd->rhs);

  int sz = strlen((char*)bnd->id) + strlen(right) + 5;
  result = (char*)calloc(sz, sizeof(char));

  strcat(result, (char*)bnd->id);
  strcat(result, clneq);
  strcat(result, right);
  return result;
}


TypeJudgement GetypeBind(Bind* bnd, Environment* env)
{
  TypeJudgement result;
  Ast* bound_term = lookup_in_local_scope(env->scope, bnd->id);

  if (bound_term == NULL)
  {
    TypeJudgement rhsjdgmt = Getype(bnd->rhs, env);
    // when we add a sequence (';') grapheme, we need to
    // bind to the type of the rhs to properly
    // typecheck terms in our grammar.
    result = rhsjdgmt;
  }
  else
  {
    result.success = false;
    result.error.dsc = "Id is already bound in scope";
    result.error.loc = bnd->loc;
  }
  return result;
}
