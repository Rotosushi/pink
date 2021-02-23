#include <stdlib.h>
#include <string.h>

#include "Ast.h"
#include "Utilities.h"
#include "Environment.h"
#include "Iteration.h"

void InitializeIteration(Iteration* itr, struct Ast* cnd, struct Ast* bdy, Location* loc)
{
  itr->loc = *loc;
  itr->cnd = cnd;
  itr->bdy = bdy;
}

void DestroyIteration(Iteration* itr)
{
  DestroyAst(itr->cnd);
  DestroyAst(itr->bdy);
}

void CloneIteration(Iteration* destination, Iteration* source)
{
  destination->loc = source->loc;
  CloneAst(&(destination->cnd), source->cnd);
  CloneAst(&(destination->cnd), source->bdy);
}

char* ToStringIteration(Iteration* itr)
{
  char *result, *condtxt, *bodytxt, *whiletxt, *dotxt, *ctx;
  whiletxt = "while "; // 6
  dotxt    = " do ";   // 4
  condtxt  = ToStringAst(itr->cnd);
  bodytxt  = ToStringAst(itr->bdy);

  int sz = strlen(condtxt) + strlen(bodytxt) + 11;
  result = (char*)calloc(sz, sizeof(char));

  strkat(result, whiletxt, &ctx);
  strkat(NULL,   condtxt,  &ctx);
  strkat(NULL,   dotxt,    &ctx);
  strkat(NULL,   bodytxt,  &ctx);
  free(condtxt);
  free(bodytxt);
  return result;
}


TypeJudgement GetypeIteration(Iteration* itr, struct Environment* env)
{
  Type* booleanType = GetBooleanType(env->interned_types);
  TypeJudgement result;

  TypeJudgement cndjdgmt = Getype(itr->cnd, env);

  if (cndjdgmt.success == true)
  {
    if (cndjdgmt.type == booleanType)
    {
      TypeJudgement bdyjdgmt = Getype(itr->bdy, env);

      if (bdyjdgmt.success == true)
      {
        result.success = true;
        result.type    = GetNilType(env->interned_types);
      }
      else
      {
        result = bdyjdgmt;
      }
    }
    else
    {
      result.success   = false;
      result.error.dsc = "condition must have type:[Bool]";
      result.error.loc = *GetAstLocation(itr->cnd);
    }
  }
  else
  {
    result = cndjdgmt;
  }

  return result;
}

EvalJudgement EvaluateIteration(Iteration* itr, struct Environment* env)
{
  EvalJudgement result;

  if (result.success == true)
    return Evaluate(result.term, env);
  else
    return result;
}
