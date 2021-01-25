#include <stdlib.h>
#include <string.h>

#include "Ast.h"
#include "Application.h"

void DestroyApplication(Application* app)
{
  DestroyAst(app->lhs);
  DestroyAst(app->rhs);
  free(app);
  app = NULL;
}

Application* CloneApplication(Application* app)
{
  Application* result = (Application*)malloc(sizeof(Application));
  result->lhs = CloneAst(app->lhs);
  result->rhs = CloneAst(app->rhs);
  return result;
}

char* ToStringApplication(Application* app)
{
  char *result, *lparen, *rparen, *spc, *left, *right;
  lparen = "(";
  rparen = ")";
  spc    = " "; // 3 characters, plus the nullchar at the end makes 4
  left   = ToStringAst(app->lhs);
  right  = ToStringAst(app->rhs);

  int sz = strlen(left) + strlen(right) + 4;
  result = (char*)calloc(sz, sizeof(char));

  strcat(result, lparen);
  strcat(result, left);
  strcat(result, spc);
  strcat(result, right);
  strcat(result, rparen);
  return result;
}
