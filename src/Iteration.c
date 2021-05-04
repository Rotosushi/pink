#include <stdlib.h>
#include <string.h>

#include "Ast.h"
#include "Utilities.hpp"
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


Judgement GetypeIteration(Iteration* itr, struct Environment* env)
{
  Location dummy;
  Ast* booleanType = CreateAstType(GetBooleanType(env->interned_types), &dummy);
  Judgement result, eqcnd;

  Judgement cndjdgmt = Getype(itr->cnd, env);

  if (cndjdgmt.success == true)
  {
    eqcnd = Equals(cndjdgmt.term, booleanType);
    if (eqcnd.success == true)
    {
      if (eqcnd.term->obj.boolean.value == true)
      {
        Judgement bdyjdgmt = Getype(itr->bdy, env);

        if (bdyjdgmt.success == true)
        {
          result.success = true;
          result.term    = CreateAstType(GetNilType(env->interned_types), &itr->loc);
        }
        else
        {
          result = bdyjdgmt;
        }
      }
      else
      {
        char* c0 = "Condition has Type:[";
        char* c1 = "] must have type:[Bool]";
        char* c  = ToStringAst(cndjdgmt.term);
        char* i0 = appendstr(c0, c);
        result.success   = false;
        result.error.dsc = appendstr(i0, c1);
        result.error.loc = *GetAstLocation(itr->cnd);
        free(c);
        free(i0);
      }
    }
    else
    {
      // we couldn't compare for some reason,
      // it's stored in eqcnd.
      result = eqcnd;
    }
  }
  else
  {
    result = cndjdgmt;
  }
  free(booleanType);
  return result;
}

Judgement EvaluateIteration(Iteration* itr, struct Environment* env)
{
  Location dummy;
  Judgement result, cndjdgmt = Evaluate(itr->cnd, env);

  if (cndjdgmt.success == true)
  {
    if (cndjdgmt.term->obj.boolean.value == true)
    {
      do
      {
        result = Evaluate(itr->bdy, env);

        if (result.success == false)
          break;

        cndjdgmt = Evaluate(itr->cnd, env);

        if (cndjdgmt.success == false)
        {
          result = cndjdgmt;
          break;
        }

      } while (cndjdgmt.term->obj.boolean.value == true);
    }
  }
  else
  {
    result = cndjdgmt;
  }


  if (result.success == true)
  {
      DestroyAst(result.term);
      result.term = CreateAstNil(&dummy);
      return result;
  }
  else
    return result;
}

bool AppearsFreeIteration(Iteration* itr, InternedString id)
{
  bool c = AppearsFree(itr->cnd, id);
  bool b = AppearsFree(itr->bdy, id);
  return c || b;
}

void RenameBindingIteration(Iteration* itr, InternedString target, InternedString replacement)
{
  RenameBinding(itr->cnd, target, replacement);
  RenameBinding(itr->bdy, target, replacement);
}

void SubstituteIteration(Iteration* itr, Ast** target, InternedString id, Ast* value, struct Environment* env)
{
  Substitute(itr->cnd, &(itr->cnd), id, value, env);
  Substitute(itr->bdy, &(itr->bdy), id, value, env);
}
