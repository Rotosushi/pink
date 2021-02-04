#include <stdlib.h>
#include <string.h>

#include "Ast.h"
#include "Assignment.h"

Assignment* CreateAssignment(struct Ast* left, struct Ast* right)
{
  Assignment* result = (Assignment*)malloc(sizeof(Assignment));
  result->lhs = left;
  result->rhs = right;
  return result;
}

void DestroyAssignment(Assignment* ass)
{
  DestroyAst(ass->lhs);
  DestroyAst(ass->rhs);
  free(ass);
  ass = NULL;
}

Assignment* CloneAssignment(Assignment* ass)
{
  Assignment* result = (Assignment*)malloc(sizeof(Assignment));
  result->lhs = CloneAst(ass->lhs);
  result->rhs = CloneAst(ass->rhs);
  return result;
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
