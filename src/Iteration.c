#include <stdlib.h>
#include <string.h>

#include "Ast.h"
#include "Iteration.h"

void DestroyIteration(Iteration* itr)
{
  DestroyAst(itr->cnd);
  DestroyAst(itr->bdy);
  free(itr);
  itr = NULL;
}

Iteration* CloneIteration(Iteration* itr)
{
  Iteration* result = (Iteration*)malloc(sizeof(Iteration));
  result->cnd = CloneAst(itr->cnd);
  result->bdy = CloneAst(itr->bdy);
  return result;
}

char* ToStringIteration(Iteration* itr)
{
  char *result, *condtxt, *bodytxt, *whiletxt, *dotxt;
  whiletxt = "while "; // 6
  dotxt    = " do ";   // 4
  condtxt  = ToStringAst(itr->cnd);
  bodytxt  = ToStringAst(itr->bdy);

  int sz = strlen(condtxt) + strlen(bodytxt) + 11;
  result = (char*)calloc(sz, sizeof(char));

  strcat(result, whiletxt);
  strcat(result, condtxt);
  strcat(result, dotxt);
  strcat(result, bodytxt);
  return result;
}
