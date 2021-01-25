#include <stdlib.h>
#include <string.h>

#include "Ast.h"
#include "Conditional.h"

void DestroyConditional(Conditional* cond)
{
  DestroyAst(cond->cnd);
  DestroyAst(cond->fst);
  DestroyAst(cond->snd);
  free(cond);
  cond = NULL;
}

Conditional* CloneConditional(Conditional* cond)
{
  Conditional* result = (Conditional*)malloc(sizeof(Conditional));
  result->cnd = CloneAst(cond->cnd);
  result->fst = CloneAst(cond->fst);
  result->snd = CloneAst(cond->snd);
  return result;
}

char* ToStringConditional(Conditional* cond)
{
  char *result, *iftxt, *condtxt, *thentxt;
  char *firsttxt, *elsetxt, *secondtxt;
  iftxt     = "if "; // 3
  thentxt   = " then " // 6
  elsetxt   = " else " // 6
  condtxt   = ToStringAst(cond->cnd);
  firsttxt  = ToStringAst(cond->fst);
  secondtxt = ToStringAst(cond->snd);

  // 15 characters and a null terminator walk into this string
  int sz = strlen(condtxt) + strlen(firsttxt) + strlen(secondtxt) + 16;
  result = (char*)calloc(sz, sizeof(char));

  strcat(result, iftxt);
  strcat(result, condtxt);
  strcat(result, thentxt);
  strcat(result, firsttxt);
  strcat(result, elsetxt);
  strcat(result, secondtxt);
  return result;
}
