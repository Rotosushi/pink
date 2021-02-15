#include <stdlib.h>
#include <string.h>

#include "Ast.h"
#include "Environment.h"
#include "Assignment.h"

void InitializeAssignment(Assignment* ass, struct Ast* left, struct Ast* right)
{
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
  CloneAst(&(dest->lhs), source->lhs);
  CloneAst(&(dest->rhs), source->rhs);
}

char* ToStringAssignment(Assignment* ass)
{
  char *result, *larrow, *left, *right;
  larrow = " <- "; // 4 chars here, plus the null terminator makes 5
  left   = ToStringAst(ass->lhs);
  right  = ToStringAst(ass->rhs);

  int sz = strlen(left) + strlen(right) + 5;
  result = (char*)calloc(sz, sizeof(char));

  strcat(result, left);
  strcat(result, larrow);
  strcat(result, right);
  free(left);
  free(right);
  return result;
}

/*
   ENV |- lhs : T, rhs : T
------------------------------
   ENV |- lhs '<-' rhs : T
*/
TypeJudgement GetypeAssignment(Ast* node, Environment* env)
{
  TypeJudgement result;

  Assignment* ass = &(node->ass);

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
        result.error.loc = node->loc;
      }
    }
    else
      result = rhsjdgmt;
  }
  else
    result = lhsjdgmt;

  return result;
}
