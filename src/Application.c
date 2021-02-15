#include <stdlib.h>
#include <string.h>

#include "Ast.h"
#include "TypeJudgement.h"
#include "Environment.h"
#include "Application.h"

void InitializeApplication(Application* app, struct Ast* left, struct Ast* right)
{
  app->lhs = left;
  app->rhs = right;
}

void DestroyApplication(Application* app)
{
  DestroyAst(app->lhs);
  DestroyAst(app->rhs);
}

void CloneApplication(Application* destination, Application* source)
{
  CloneAst(&(destination->lhs), source->lhs);
  CloneAst(&(destination->rhs), source->rhs);
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
  free(left);
  free(right);
  return result;
}

/*
   ENV |- lhs : t1 -> t2, rhs : t1
   --------------------------------------------
           ENV |- lhs rhs : t2
*/
TypeJudgement GetypeApplication(struct Ast* node, struct Environment* env)
{
  if (!node || !env)
    FatalError("Bad Getype Call", __FILE__, __LINE__);

  // pull out a pointer to the member data
  Application* app = &(node->app);

  TypeJudgement result;

  TypeJudgement lhsjdgmt = Getype(app->lhs, env);

  if (lhsjdgmt.success == true)
  {
    Type *lhstype = lhsjdgmt.type;

    if (lhstype->kind == T_PROC)
    {
      ProcType *pt = &(lhstype->proc);
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
          result.error.loc = node->loc;
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
