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
Judgement GetypeConditional(Conditional* conditional, Environment* env)
{
  Ast* booleanType = GetBooleanType(env->interned_types, NULL);
  Judgement result, eqcnd, fstjdgmt, sndjdgmt, eqalts;

  Judgement cndjdgmt = Getype(conditional->cnd, env);

  eqcnd = Equals(cndjdgmt.term, booleanType);

  if (eqcnd.success == false)
  {
    free(booleanType);
    result = eqcnd;
    return result;
  }

  if (eqcnd.term->obj.boolean.value == false)
  {
    char* c0 = "Conditional espression has Type:[";
    char* c1 = "] must have Type:[Bool]";
    char* c  = ToStringAst(cndjdgmt.term);
    char* i0 = appendstr(c0, c);
    result.success   = false;
    result.error.loc = *GetAstLocation(conditional->cnd);
    result.error.dsc = appendstr(i0, c1);
    free(c);
    free(i0);
    free(booleanType);
    return result;
  }

  fstjdgmt = Getype(conditional->fst, env);

  // if the typing failed, return the reason.
  if (fstjdgmt.success == false)
  {
    free(booleanType);
    result = fstjdgmt;
    return result;
  }

  sndjdgmt = Getype(conditional->snd, env);

  if (sndjdgmt.success == false)
  {
    free(booleanType);
    result = sndjdgmt;
    return result;
  }

  eqalts = Equals(fstjdgmt.term, sndjdgmt.term);

  // if the comparison failed, return the reason.
  if (eqalts.success == false)
  {
    free(booleanType);
    result = eqalts;
    return result;
  }

  // if the alternative expressions had equal types
  // then we type the conditional expression as the
  // type of the first alternative.
  if (eqalts.term->obj.boolean.value == true)
  {
    result = fstjdgmt;
  }
  else
  {
    char* c0 = "Alternative expressions have differing types lhs:[";
    char* c1 = "] rhs:[";
    char* c2 = "]";
    char* f  = ToStringAst(fstjdgmt.term);
    char* s  = ToStringAst(sndjdgmt.term);
    char* i0 = appendstr(c0, f);
    char* i1 = appendstr(i0, c1);
    char* i2 = appendstr(i1, s);
    result.success   = false;
    result.error.loc = *GetAstLocation(conditional->fst);
    result.error.dsc = appendstr(i2, c2);
    free(f);
    free(s);
    free(i0);
    free(i1);
    free(i2);
  }

  free(booleanType);
  return result;
}

Judgement EvaluateConditional(Conditional* cond, struct Environment* env)
{
  Judgement result, cndjdgmt;
  Location dummy;
  Ast* literalTrue = CreateAstBoolean(true, &dummy);

  cndjdgmt = Evaluate(cond->cnd, env);

  if (cndjdgmt.success == true)
  {
    Judgement truthjdgmt = Equals(cndjdgmt.term, literalTrue);
    if (truthjdgmt.success)
    {
      if (truthjdgmt.term->obj.boolean.value == true)
        result = Evaluate(cond->fst, env);
      else
        result = Evaluate(cond->snd, env);
    }
    else
    {
      result = truthjdgmt;
    }
  }
  else
  {
    result = cndjdgmt;
  }

  free(literalTrue);

  return result;
}

bool AppearsFreeConditional(Conditional* cond, InternedString id)
{
  bool c = AppearsFree(cond->cnd, id);
  bool f = AppearsFree(cond->fst, id);
  bool s = AppearsFree(cond->snd, id);
  return c || f || s;
}

void RenameBindingConditional(Conditional* cond, InternedString target, InternedString replacement)
{
  RenameBinding(cond->cnd, target, replacement);
  RenameBinding(cond->fst, target, replacement);
  RenameBinding(cond->snd, target, replacement);
}

void SubstituteConditional(Conditional* cond, Ast** target, InternedString id, Ast* value, Environment* env)
{
  Substitute(cond->cnd, &(cond->cnd), id, value, env);
  Substitute(cond->fst, &(cond->fst), id, value, env);
  Substitute(cond->snd, &(cond->snd), id, value, env);
}










//
