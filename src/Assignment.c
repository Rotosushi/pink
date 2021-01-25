#include <stdlib.h>
#include <string.h>

#include "Ast.h"
#include "Assignment.h"

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
