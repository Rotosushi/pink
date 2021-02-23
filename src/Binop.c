#include <stdlib.h>
#include <string.h>

#include "Ast.h"
#include "Utilities.h"
#include "StringInterner.h"
#include "Environment.h"
#include "Binop.h"

void InitializeBinop(Binop* binop, InternedString op, struct Ast* left, struct Ast* right, Location* loc)
{
  binop->loc = *loc;
  binop->op  = op;
  binop->lhs = left;
  binop->rhs = right;
}

void DestroyBinop(Binop* binop)
{
  DestroyAst(binop->lhs);
  DestroyAst(binop->rhs);
}

void CloneBinop(Binop* destination, Binop* source)
{
  destination->loc = source->loc;
  destination->op = source->op;
  CloneAst(&(destination->lhs), source->lhs);
  CloneAst(&(destination->rhs), source->rhs);
}

char* ToStringBinop(Binop* binop)
{
  char *result, *spc, *left, *right, *ctx;
  spc   = " ";
  left  = ToStringAst(binop->lhs);
  right = ToStringAst(binop->rhs);

  int sz = strlen((char*)binop->op)
         + strlen(left)
         + strlen(right)
         + 3; // ...2 spaces and a null terminator walk into this string
  result = (char*)calloc(sz, sizeof(char));

  strkat(result, left,  &ctx);
  strkat(result, spc,   &ctx);
  strkat(result, (char*)binop->op, &ctx);
  strkat(result, spc,   &ctx);
  strkat(result, right, &ctx);
  free(left);
  free(right);
  return result;
}

// if we can find the binop bound within the environment,
// and type the left and right hand sides,
// and the binop has an eliminator for the given
// types of the left and right hand sides, we can type
// the entire expression as the result type of the
// given eliminator.
TypeJudgement GetypeBinop(Binop* binop, Environment* env)
{
  TypeJudgement result;

  BinopEliminatorList* eliminators = FindBinop(env->binops, binop->op);

  if (eliminators != NULL)
  {
    TypeJudgement lhsjdgmt = Getype(binop->lhs, env);

    if (lhsjdgmt.success == true)
    {
      TypeJudgement rhsjdgmt = Getype(binop->rhs, env);

      if (rhsjdgmt.success == true)
      {
        BinopEliminator* eliminator = FindBinopEliminator(eliminators, lhsjdgmt.type, rhsjdgmt.type);

        if (eliminator != NULL)
        {
          result.success = true;
          result.type    = eliminator->restype;
        }
        else
        {
          result.success   = false;
          result.error.dsc = "no implementation of binop found for actual types";
          result.error.loc = binop->loc;
        }
      }
      else
      {
        result = rhsjdgmt;
      }
    }
    else
    {
      result = lhsjdgmt;
    }
  }
  else
  {
    result.success = false;
    result.error.dsc = "binop not bound within environment";
    result.error.loc = binop->loc;
  }

  return result;
}
