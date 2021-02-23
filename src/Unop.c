#include <stdlib.h>
#include <string.h>

#include "Ast.h"
#include "Utilities.h"
#include "Environment.h"
#include "StringInterner.h"
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

TypeJudgement GetypeUnop(Unop* uop, Environment* env)
{
  TypeJudgement result;

  UnopEliminatorList* eliminators = FindUnop(env->unops, uop->op);

  if (eliminators != NULL)
  {
    TypeJudgement rhsjdgmt = Getype(uop->rhs, env);

    if (rhsjdgmt.success == true)
    {
      UnopEliminator* eliminator = FindUnopEliminator(eliminators, rhsjdgmt.type);

      if (eliminator != NULL)
      {
        result.success = true;
        result.type    = eliminator->restype;
      }
      else
      {
        result.success   = false;
        result.error.dsc = "no member of unop found for actual type";
        result.error.loc = *GetAstLocation(uop->rhs);
      }
    }
    else
    {
      result = rhsjdgmt;
    }
  }
  else
  {
    result.success   = false;
    result.error.dsc = "unop not bound within the environment";
    result.error.loc = uop->loc;
  }

  return result;
}

EvalJudgement EvaluateUnop(Unop* uop, struct Environment* env)
{
  EvalJudgement result;

  if (result.success == true)
    return Evaluate(result.term, env);
  else
    return result;
}
