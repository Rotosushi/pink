#include <stdlib.h>
#include <string.h>

#include "Ast.h"
#include "Utilities.h"
#include "Location.h"
#include "Environment.h"
#include "Conditional.h"

void InitializeConditional(Conditional* cond, struct Ast* cnd, struct Ast* fst, struct Ast* snd, Location* loc)
{
  cond->loc = *loc;
  cond->cnd = cnd;
  cond->fst = fst;
  cond->snd = snd;
}

void DestroyConditional(Conditional* cond)
{
  DestroyAst(cond->cnd);
  DestroyAst(cond->fst);
  DestroyAst(cond->snd);
}

void CloneConditional(Conditional* destination, Conditional* source)
{
  destination->loc = source->loc;
  CloneAst(&(destination->cnd), source->cnd);
  CloneAst(&(destination->fst), source->fst);
  CloneAst(&(destination->snd), source->snd);
}

char* ToStringConditional(Conditional* cond)
{
  char *result, *iftxt, *condtxt, *thentxt, *ctx;
  char *firsttxt, *elsetxt, *secondtxt;
  iftxt     = "if "; // 3
  thentxt   = " then "; // 6
  elsetxt   = " else "; // 6
  condtxt   = ToStringAst(cond->cnd);
  firsttxt  = ToStringAst(cond->fst);
  secondtxt = ToStringAst(cond->snd);

  // 15 characters and a null terminator walk into this string
  int sz = strlen(condtxt) + strlen(firsttxt) + strlen(secondtxt) + 16;
  result = (char*)calloc(sz, sizeof(char));

  strkat(result, iftxt,   &ctx);
  strkat(NULL, condtxt,   &ctx);
  strkat(NULL, thentxt,   &ctx);
  strkat(NULL, firsttxt,  &ctx);
  strkat(NULL, elsetxt,   &ctx);
  strkat(NULL, secondtxt, &ctx);
  free(condtxt);
  free(firsttxt);
  free(secondtxt);
  return result;
}

// the condition has to have type Bool.
// the alternatives must have equal types.
// the result type is the type of the alternatives
TypeJudgement GetypeConditional(Conditional* conditional, Environment* env)
{
  Type* booleanType = GetBooleanType(env->interned_types);
  TypeJudgement result;

  TypeJudgement cndjdgmt = Getype(conditional->cnd, env);

  if (cndjdgmt.success == true)
  {
    if (cndjdgmt.type == booleanType)
    {
      TypeJudgement fstjdgmt = Getype(conditional->fst, env);

      if (fstjdgmt.success == true)
      {
        TypeJudgement sndjdgmt = Getype(conditional->snd, env);

        if (sndjdgmt.success == true)
        {
          if (fstjdgmt.type == sndjdgmt.type)
          {
            result.success = true;
            result.type    = fstjdgmt.type;
          }
          else
          {
            result.success   = false;
            result.error.dsc = "alternative expressions must have the same type";
            result.error.loc = *GetAstLocation(conditional->fst);
          }
        }
        else
        {
          result = sndjdgmt;
        }
      }
      else
      {
        result = fstjdgmt;
      }
    }
    else
    {
      result.success   = false;
      result.error.dsc = "conditional expression must have boolean type";
      result.error.loc = *GetAstLocation(conditional->cnd);
    }
  }
  else
  {
    result = cndjdgmt;
  }
  return result;
}
