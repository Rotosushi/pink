#include <stdlib.h>
#include <string.h>

#include "Ast.h"
#include "Judgement.h"
#include "Utilities.hpp"
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
        Type* lhstype = lhsjdgmt.term->obj.type.literal;
        Type* rhstype = rhsjdgmt.term->obj.type.literal;
        BinopEliminator* eliminator = FindBinopEliminator(eliminators, lhstype, rhstype);

        if (eliminator != NULL)
        {
          result.success = true;
          result.term    = CreateAstType(eliminator->restype, &dummy);
        }
        else
        {
          char* c0 = "no implementation of binop [";
          char* c1 = "] found for actual types left:[";
          char* c2 = "], right:[";
          char* c3 = "]";
          char* l  = ToStringAst(lhsjdgmt.term);
          char* r  = ToStringAst(rhsjdgmt.term);
          char* t0 = appendstr(c0, binop->op);
          char* t1 = appendstr(t0, c1);
          char* t2 = appendstr(t1, l);
          char* t3 = appendstr(t2, c2);
          char* t4 = appendstr(t3, r);
          result.success   = false;
          result.error.dsc = appendstr(t4, c3);
          result.error.loc = binop->loc;
          free(l);
          free(r);
          free(t0);
          free(t1);
          free(t2);
          free(t3);
          free(t4);
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
    char* c0 = "operator [";
    char* c1 = "] is not bound as binop in environment";
    char* t0 = appendstr(c0, binop->op);
    result.success   = false;
    result.error.dsc = appendstr(t0, c1);
    result.error.loc = binop->loc;
    free(t0);
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

        //              (  ast handle   ) ( type handle  )
        Type *lhstype = lhstypejdgmt.term->obj.type.literal;
        Type *rhstype = rhstypejdgmt.term->obj.type.literal;

        BinopEliminator* eliminator = FindBinopEliminator(eliminators, lhstype, rhstype);

        if (eliminator != NULL)
        {
          result = (eliminator->eliminator)(lhsjdgmt.term, rhsjdgmt.term);
        }
        else
        {
          char* c0 = "no implementation of binop found for actual lhs:[";
          char* c1 = "] and rhs:[";
          char* c2 = "]";
          char* l  = ToStringAst(lhstypejdgmt.term);
          char* r  = ToStringAst(rhstypejdgmt.term);
          char* i0 = appendstr(c0, l);
          char* i1 = appendstr(i0, c1);
          char* i2 = appendstr(i1, r);
          result.success   = false;
          result.error.dsc = appendstr(i2, c2);
          result.error.loc = binop->loc;
          free(l);
          free(r);
          free(i0);
          free(i1);
          free(i2);
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
    char* c0 = "binop [";
    char* c1 = "] not bound within environment";
    char* i0 = appendstr(c0, (char*)binop->op);
    result.success = false;
    result.error.dsc = appendstr(i0, c1);
    result.error.loc = binop->loc;
    free(i0);
  }
  return result;
}

bool AppearsFreeBinop(Binop* binop, InternedString id)
{
  // we don't want to short circuit the
  // evaluation of the second recursive
  // application.
  bool l = AppearsFree(binop->lhs, id);
  bool r = AppearsFree(binop->rhs, id);
  return l || r;
}

void RenameBindingBinop(Binop* binop, InternedString target, InternedString replacement)
{
  RenameBinding(binop->lhs, target, replacement);
  RenameBinding(binop->rhs, target, replacement);
}

void SubstituteBinop(Binop* binop, Ast** target, InternedString id, Ast* value, struct Environment* env)
{
  Substitute(binop->lhs, &(binop->lhs), id, value, env);
  Substitute(binop->rhs, &(binop->rhs), id, value, env);
}
