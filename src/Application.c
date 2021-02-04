#include <stdlib.h>
#include <string.h>

#include "Ast.h"
#include "TypeJudgement.h"
#include "Environment.h"
#include "Application.h"

Application* CreateApplication(struct Ast* left, struct Ast* right)
{
  Application* result = (Application*)malloc(sizeof(Application));
  result->lhs = left;
  result->rhs = right;
  return result;
}

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

/*
   ENV |- lhs : t1 -> t2, rhs : t1
   --------------------------------------------
           ENV |- lhs rhs : t2
*/
TypeJudgement GetypeApplication(Application* app, struct Environment* env)
{
  if (!app || !env)
    FatalError("Bad Getype Call", __FILE__, __LINE__);


  TypeJudgement result;

  TypeJudgement lhsjdgmt = Getype(app->lhs, env);

  if (lhsjdgmt.success == true)
  {
    Type *lhstype = lhsjdgmt.type;

    if (lhstype->kind == T_PROC)
    {
      ProcType *pt = lhstype.proc;
      Type     *t1 = pt->lhs, *t2 = pt->rhs;

      TypeJudgement rhsjdgmt = Getype(app->rhs, env);

      if (rhsjdgmt.success == true)
      {
        Type* t3 = rhsjdgmt.type;

        if (t1 == t3)
        {
          result.success = true;
          result.type    = t2;
        }
        else
        {
          // TODO: better error messages.
          result.success   = false;
          result.error.dsc = "Type mismatch between formal and actual parameters";
          result.error.loc = app->loc;
        }
      }
      else
        result = rhsjdgmt;
    }
    else
    {
      result.success   = false;
      result.error.dsc = "Cannot apply non-procedure lhs type";
      result.error.loc = app->lhs->loc;
    }
  }
  else
    result = lhsjdgmt;

  return result;
}
