#include <stdlib.h>
#include <string.h>

#include "Ast.h"
#include "StringInterner.h"
#include "Environment.h"
#include "Binop.h"

void CreateBinop(Binop* binop, InternedString op, struct Ast* left, struct Ast* right)
{
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
  destination->op = source->op;
  CloneAst((*destination)->lhs, source->lhs);
  CloneAst((*destination)->rhs, source->rhs);
}

char* ToStringBinop(Binop* binop)
{
  char *result, *spc, *left, *right;
  spc   = " ";
  left  = ToStringAst(binop->lhs);
  right = ToStringAst(binop->rhs);

  int sz = strlen((char*)binop->op)
         + strlen(left)
         + strlen(right)
         + 3; // ...2 spaces and a null terminator walk into this string
  result = (char*)calloc(sz, sizeof(char));

  strcat(result, left);
  strcat(result, spc);
  strcat(result, (char*)binop->op);
  strcat(result, spc);
  strcat(result, right);
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
        BinopEliminator* eliminator = FindBinopEliminator(env->eliminators, lhsjdgmt.type, rhsjdgmt.type);

        if (eliminator != NULL)
        {
          result.success = true;
          result.type    = eliminator->restype;
        }
        else
        {
          Location *lhsloc = &(binop->lhs->loc), *rhsloc = &(binop->rhs->loc);
          result.success   = false;
          result.error.dsc = "no member of binop found for actual type";
          result.error.loc.first_line   = lhsloc->first_line;
          result.error.loc.first_column = lhsloc->first_column;
          result.error.loc.last_line    = rhsloc->last_line;
          result.error.loc.last_column  = rhsloc->last_column;
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
    Location *lhsloc = &(binop->lhs->loc), *rhsloc = &(binop->rhs->loc);
    // I forgot that location data is stored within the Ast node,
    // but we pass the member into this procedure, meaning there
    // is no easy way to get to the location of the binop expression
    // in it's entirety for easy code. so, recall that the operator
    // is between the left and right hand sides and so the location
    // is also!
    result.error.loc.first_line   = lhsloc->last_line;
    result.error.loc.first_column = lhsloc->last_column;
    result.error.loc.last_line    = rhsloc->first_line;
    result.error.loc.last_column  = rhsloc->first_column;

    // to 'solve' this problem, we can either
    // A) move location structs into each member.
    // this modifies construction semantics slightly,
    // which is a breaking change.
    // B) pass in Ast*'s into GetypeBinop() procedures.
    // then, we pull the member out of the Ast
    // within the body, assured by the path of
    // execution that this ast holds a valid pointer
    // to it's member data.
  }

  return result;
}
