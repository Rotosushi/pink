#include <stdlib.h>
#include <string.h>

#include "Ast.h"
#include "Judgement.h"
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
Judgement GetypeBinop(Binop* binop, Environment* env)
{
  Location  dummy;
  Judgement result;

  BinopEliminatorList* eliminators = FindBinop(env->binops, binop->op);

  if (eliminators != NULL)
  {
    Judgement lhsjdgmt = Getype(binop->lhs, env);

    if (lhsjdgmt.success == true)
    {
      Judgement rhsjdgmt = Getype(binop->rhs, env);

      if (rhsjdgmt.success == true)
      {
        BinopEliminator* eliminator = FindBinopEliminator(eliminators, lhsjdgmt.term, rhsjdgmt.term);

        if (eliminator != NULL)
        {
          result.success = true;
          result.term    = CreateAstType(eliminator->restype, &dummy);
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

Judgement EvaluateBinop(Binop* binop, struct Environment* env)
{
  Judgement result;

  BinopEliminatorList* eliminators = FindBinop(env->binops, binop->op);

  if (eliminators != NULL)
  {
    Judgement lhsjdgmt = Evaluate(binop->lhs, env);

    if (lhsjdgmt.success == true)
    {
      Judgement rhsjdgmt = Evaluate(binop->rhs, env);

      if (rhsjdgmt.success == true)
      {
        Judgement lhstypejdgmt = Getype(lhsjdgmt.term, env);

        if (lhstypejdgmt.success == false)
        {
          result = lhstypejdgmt;
          return result;
        }

        Judgement rhstypejdgmt = Getype(rhsjdgmt.term, env);

        if (rhstypejdgmt.success == false)
        {
          result = rhstypejdgmt;
          return result;
        }

        Type *lhstype = lhstypejdgmt.term->obj.type.literal;
        Type *rhstype = rhstypejdgmt.term->obj.type.literal;

        BinopEliminator* eliminator = FindBinopEliminator(eliminators, lhstype, rhstype);

        if (eliminator != NULL)
        {
          result = (eliminator->eliminator)(lhsjdgmt.term, rhsjdgmt.term);
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

  if (result.success == true)
    return Evaluate(result.term, env);
  else
    return result;
}
