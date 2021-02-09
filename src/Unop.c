#include <stdlib.h>
#include <string.h>

#include "Ast.h"
#include "StringInterner.h"
#include "Unop.h"

void InitializeUnop(Unop* unop, InternedString op, struct Ast* rhs)
{
  unop->op  = op;
  unop->rhs = rhs;
}

void DestroyUnop(Unop* unop)
{
  DestroyAst(unop->rhs);
}

void CloneUnop(Unop* destination, Unop* source)
{
  result->op = unop->op;
  CloneAst(&(destination->rhs), source->rhs);
}

char* ToStringUnop(Unop* unop)
{
  char *result, *spc, *right;
  spc   = " ";
  right = ToStringAst(unop->rhs);

  int sz = strlen((char*)unop->op) + strlen(right) + 2;
  result = (char*)calloc(sz, sizeof(char));

  strcat(result, (char*)unop->op);
  strcat(result, spc);
  strcat(result, right);
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
        result.error.loc = uop->rhs->loc;
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
    result.error.loc = uop->rhs->loc;
  }

  return result;
}
