#include <stdlib.h>
#include <string.h>

#include "Ast.h"
#include "Environment.h"
#include "Iteration.h"

void InitializeIteration(Iteration* itr, struct Ast* cnd, struct Ast* bdy)
{
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
  CloneAst(&(destination->cnd), source->cnd);
  CloneAst(&(destination->cnd), source->bdy);
}

char* ToStringIteration(Iteration* itr)
{
  char *result, *condtxt, *bodytxt, *whiletxt, *dotxt;
  whiletxt = "while "; // 6
  dotxt    = " do ";   // 4
  condtxt  = ToStringAst(itr->cnd);
  bodytxt  = ToStringAst(itr->bdy);

  int sz = strlen(condtxt) + strlen(bodytxt) + 11;
  result = (char*)calloc(sz, sizeof(char));

  strcat(result, whiletxt);
  strcat(result, condtxt);
  strcat(result, dotxt);
  strcat(result, bodytxt);
  free(condtxt);
  free(bodytxt);
  return result;
}


TypeJudgement GetypeIteration(struct Ast* node, struct Environment* env)
{
  Type* booleanType = GetBooleanType(env->interned_types);
  TypeJudgement result;
  Iteration* itr = &(node->itr);
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
      result.error.loc = itr->cnd->loc;
    }
  }
  else
  {
    result = cndjdgmt;
  }

  return result;
}
