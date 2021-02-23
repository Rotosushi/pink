#include <stdlib.h>
#include <string.h>

#include "Ast.h"
#include "Utilities.h"
#include "Location.h"
#include "Environment.h"
#include "Assignment.h"

void InitializeAssignment(Assignment* ass, struct Ast* left, struct Ast* right, Location* loc)
{
  ass->loc = *loc;
  ass->lhs = left;
  ass->rhs = right;
}

void DestroyAssignment(Assignment* ass)
{
  DestroyAst(ass->lhs);
  DestroyAst(ass->rhs);
}

void CloneAssignment(Assignment* dest, Assignment* source)
{
  dest->loc = source->loc;
  CloneAst(&(dest->lhs), source->lhs);
  CloneAst(&(dest->rhs), source->rhs);
}

char* ToStringAssignment(Assignment* ass)
{
  char *result, *larrow, *left, *right, *ctx;
  larrow = " <- "; // 4 chars here, plus the null terminator makes 5
  left   = ToStringAst(ass->lhs);
  right  = ToStringAst(ass->rhs);

  int sz = strlen(left) + strlen(right) + 5;
  result = (char*)calloc(sz, sizeof(char));

  strkat(result, left,   &ctx);
  strkat(result, larrow, &ctx);
  strkat(result, right,  &ctx);
  free(left);
  free(right);
  return result;
}

/*
   ENV |- lhs : T, rhs : T
------------------------------
   ENV |- lhs '<-' rhs : T
*/
TypeJudgement GetypeAssignment(Assignment* ass, Environment* env)
{
  TypeJudgement result;

  TypeJudgement lhsjdgmt = Getype(ass->lhs, env);

  if (lhsjdgmt.success == true)
  {
    TypeJudgement rhsjdgmt = Getype(ass->rhs, env);

    if (rhsjdgmt.success == true)
    {
      if (lhsjdgmt.type == rhsjdgmt.type)
        result = lhsjdgmt;
      else
      {
        result.success   = false;
        result.error.dsc = "Type mismatch between lhs and rhs";
        result.error.loc = ass->loc;
      }
    }
    else
      result = rhsjdgmt;
  }
  else
    result = lhsjdgmt;

  return result;
}

EvalJudgement EvaluateAssignment(Assignment* ass, Environment* env)
{
  EvalJudgement result;

  // evaluate the assignment

  if (result.success == true)
    return Evaluate(result.term, env);
  else
    return result;
}
