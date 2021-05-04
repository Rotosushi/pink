#include <stdlib.h>
#include <string.h>

#include "Ast.h"
#include "Utilities.hpp"
#include "Environment.h"
#include "StringInterner.hpp"
#include "UnopEliminators.h"
#include "Unop.h"

void InitializeUnop(Unop* unop, InternedString op, struct Ast* rhs, Location* loc)
{
  unop->loc = *loc;
  unop->op  = op;
  unop->rhs = rhs;
}

void DestroyUnop(Unop* unop)
{
  DestroyAst(unop->rhs);
}

void CloneUnop(Unop* destination, Unop* source)
{
  destination->loc = source->loc;
  destination->op  = source->op;
  CloneAst(&(destination->rhs), source->rhs);
}

char* ToStringUnop(Unop* unop)
{
  char *result, *right, *ctx;
  right = ToStringAst(unop->rhs);

  int sz = strlen((char*)unop->op) + strlen(right) + 1;
  result = (char*)calloc(sz, sizeof(char));

  strkat(result, (char*)unop->op, &ctx);
  strkat(NULL,   right,           &ctx);
  free(right);
  return result;
}

Judgement GetypeUnop(Unop* uop, Environment* env)
{
  Location dummy;
  Judgement result;

  UnopEliminatorList* eliminators = FindUnop(env->unops, uop->op);

  if (eliminators != NULL)
  {
    Judgement rhsjdgmt = Getype(uop->rhs, env);

    if (rhsjdgmt.success == true)
    {
      Type* rhstype = rhsjdgmt.term->obj.type.literal;
      UnopEliminator* eliminator = FindUnopEliminator(eliminators, rhstype);

      if (eliminator != NULL)
      {
        result.success = true;
        result.term    = CreateAstType(eliminator->restype, &dummy);
      }
      else
      {
        char* c0 = "no implementation of unop [";
        char* c1 = "] found for Actual Type [";
        char* c2 = "]";
        char* a  = ToStringType(rhstype);
        char* t0 = appendstr(c0, (char*)uop->op);
        char* t1 = appendstr(t0, c1);
        char* t2 = appendstr(t1, a);
        result.success   = false;
        result.error.dsc = appendstr(t2, c2);
        result.error.loc = *GetAstLocation(uop->rhs);
        free(a);
        free(t0);
        free(t1);
        free(t2);
      }
    }
    else
    {
      result = rhsjdgmt;
    }
  }
  else
  {
    char* c0 = "unop [";
    char* c1 = "] not bound within the environment";
    char* t0 = appendstr(c0, (char*)uop->op);
    result.success   = false;
    result.error.dsc = appendstr(t0, c1);
    result.error.loc = uop->loc;
    free(t0);
  }

  return result;
}

Judgement EvaluateUnop(Unop* uop, struct Environment* env)
{
  Judgement result, rhs;

  UnopEliminatorList* eliminators = FindUnop(env->unops, uop->op);

  if (eliminators != NULL)
  {
    rhs = Evaluate(uop->rhs, env); // free me

    if (rhs.success == true)
    {
      Judgement rhstypejdgmt = Getype(rhs.term, env); // free me
      Type* rhstype = rhstypejdgmt.term->obj.type.literal;
      DestroyAst(rhstypejdgmt.term); // free'd!
      UnopEliminator* eliminator = FindUnopEliminator(eliminators, rhstype);

      if (eliminator != NULL)
      {
        result = (eliminator->eliminator)(rhs.term); // dynamic result
        DestroyAst(rhs.term);
      }
      else
      {
        char* c0 = "no implementation of unop [";
        char* c1 = "] found for Actual Type [";
        char* c2 = "]";
        char* a  = ToStringType(rhstype);
        char* i0 = appendstr(c0, (char*)uop->op);
        char* i1 = appendstr(i0, c1);
        char* i2 = appendstr(i1, a);
        result.success   = false;
        result.error.dsc = appendstr(i2, c2);
        result.error.loc = *GetAstLocation(uop->rhs);
        free(a);
        free(i0);
        free(i1);
        free(i2);
      }
    }
    else
    {
      result = rhs; // no longer need to free rhs, as we want to keep the error.
    }
  }
  else
  {
    char* c0 = "unop [";
    char* c1 = "] not bound within the environment";
    char* i0 = appendstr(c0, (char*)uop->op);
    result.success   = false;
    result.error.dsc = appendstr(i0, c1);
    result.error.loc = uop->loc;
    free(i0);
  }

  return result;
}

bool AppearsFreeUnop(Unop* uop, InternedString id)
{
  return AppearsFree(uop->rhs, id);
}

void RenameBindingUnop(Unop* uop, InternedString target, InternedString replacement)
{
  RenameBinding(uop->rhs, target, replacement);
}

void SubstituteUnop(Unop* uop, Ast** target, InternedString id, Ast* value, struct Environment* env)
{
  Substitute(uop->rhs, &(uop->rhs), id, value, env);
}
