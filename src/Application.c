#include <stdlib.h>
#include <string.h>

#include "Ast.h"
#include "Utilities.hpp"
#include "Location.h"
#include "Judgement.h"
#include "Environment.h"
#include "SymbolTable.h"
#include "Application.h"

void InitializeApplication(Application* app, struct Ast* left, struct Ast* right, int num_args, Location* loc)
{
  app->loc = *loc;
  app->num_args = num_args;
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
  destination->loc = source->loc;
  destination->num_args = source->num_args;
  CloneAst(&(destination->lhs), source->lhs);
  CloneAst(&(destination->rhs), source->rhs);
}

char* ToStringApplication(Application* app)
{
  char *result, *lparen, *rparen, *spc, *left, *right, *ctx;
  lparen = "(";
  rparen = ")";
  spc    = " "; // 3 characters, plus the nullchar at the end makes 4
  left   = ToStringAst(app->lhs);
  right  = ToStringAst(app->rhs);

  if (app->lhs->kind == A_OBJ) // then it's the lambda literal directly,
  {                            // and it looks like the argument is part
                               // of the lambda's body when we print unless
                               // we are explicit about where the lambda ends
    int sz = strlen(left) + strlen(right) + 6;
    result = (char*)calloc(sz, sizeof(char));

    strkat(result, lparen, &ctx);
    strkat(NULL,   lparen, &ctx);
    strkat(NULL,   left,   &ctx);
    strkat(NULL,   rparen, &ctx);
    strkat(NULL,   spc,    &ctx);
    strkat(NULL,   right,  &ctx);
    strkat(NULL,   rparen, &ctx);
  }
  else
  {
    int sz = strlen(left) + strlen(right) + 4;
    result = (char*)calloc(sz, sizeof(char));

    strkat(result, lparen, &ctx);
    strkat(NULL,   left,   &ctx);
    strkat(NULL,   spc,    &ctx);
    strkat(NULL,   right,  &ctx);
    strkat(NULL,   rparen, &ctx);
  }
  free(left);
  free(right);
  return result;
}

/*
   ENV |- lhs : t1 -> t2, rhs : t1
   --------------------------------------------
           ENV |- lhs rhs : t2
*/
Judgement GetypeApplication(Application* app, struct Environment* env)
{
  if (!app || !env)
    FatalError("Bad Getype Call", __FILE__, __LINE__);

  Judgement result;
  Location dummy;
  Judgement lhsjdgmt = Getype(app->lhs, env);

  if (lhsjdgmt.success == true)
  {
    Type *lhstype = lhsjdgmt.term->obj.type.literal;

    if (lhstype->kind == T_PROC)
    {
      ProcType *pt = &(lhstype->proc);
      Type     *t1 = pt->lhs, *t2 = pt->rhs;

      Judgement rhsjdgmt = Getype(app->rhs, env);

      if (rhsjdgmt.success == true)
      {
        Type* t3 = rhsjdgmt.term->obj.type.literal;

        if (t1 == t3)
        {
          result.success = true;
          result.term    = CreateAstType(t2, &dummy);
        }
        else
        {
          char* c0 = "Actual Parameter Type:[";
          char* c1 = "] doesn't match Formal Parameter Type:[";
          char* c2 = "]";
          char* a  = ToStringType(t3);
          char* f  = ToStringType(t1);
          char* i0 = appendstr(c0, a);
          char* i1 = appendstr(i0, c1);
          char* i2 = appendstr(i1, f);
          result.success   = false;
          result.error.dsc = appendstr(i2, c2);
          result.error.loc = app->loc;
        }
      }
      else
        result = rhsjdgmt;
    }
    else
    {
      char* c0 = "Cannot apply non-procedure lhs:[";
      char* c1 = "]";
      char* l  = ToStringAst(lhsjdgmt.term);
      char* i0 = appendstr(c0, l);
      result.success   = false;
      result.error.dsc = appendstr(i0, c1);
      result.error.loc = *GetAstLocation(app->lhs);
      free(l);
      free(i0);
    }
  }
  else
    result = lhsjdgmt;

  return result;
}


bool is_lambda_literal(Ast* term)
{
  bool result = false;
  if ((term->kind == A_OBJ) && (term->obj.kind == O_LAMBDA))
    result = true;
  return result;
}

bool is_partial_application(Ast* term)
{
  bool result = false;
  if ((term->kind == A_OBJ) && (term->obj.kind == O_PARAPP))
    result = true;
  return result;
}

/*
  okay, so we want to support partial application
  in an imperitive language, and my technique is simply
  going to be when an application term is not the final
  term before the start of the body, i.e. the procedure
  is curried, instead of performing the substitution immediately,
  (which works for interpretation, but not compilation), we
  instead start building up a list of (name,value) pairs,
  once we reach the final application, i.e. we are applying
  a non-curryed procedure, we do all of the substitutions from
  the list at once, along with the final substitution.
  thus, a partially applyed C procedure, is simply a pointer
  to the original procedure, plus this list of already evaluated
  arguments. we can accept this partial application in place of a
  lambda object directly, (because it is holds the next lambda object
  directly in interpretation, and is always the function pointer in
  compilation.) this partial application is valid to be stored as
  a value, with size equal to a function pointer plus the list object.
  the type of the partial application is the remaining type of
  the procedure after the already given arguments have been

*/
Judgement EvaluateApplication(Application* app, Environment* env)
{
  Judgement result, lhs, rhs;

  // evaluate the application
  // evaluate the lhs down to a callable object (maybe standardize the callable interface
  //    across all callable constructs, such that programmers can define objects
  //    which can conform to the callable interface and can thus be used by the
  //    compiler in place of the lhs of application terms. notice by definition
  //    all values are rhs's and lhs's are either an evaluatable term, a callable
  //    object, or another application term, and thus more argument list.)
  // evaluate the rhs to an object.
  // make a copy of the lambda's body for substitution.
  // Substitute the object in for the argument name
  //    within the body of the lambda.
  // evaluate the new body term and return the resulting object.

  lhs = Evaluate(app->lhs, env);

  if (lhs.success == false)
  {
    result = lhs;
    return result;
  }

  Ast* body = NULL, *value = NULL;

  if (is_lambda_literal(lhs.term))
  {
    rhs = Evaluate(app->rhs, env);

    if (rhs.success == false)
    {
      result = rhs;
      return result;
    }

    Lambda* lambda = &(lhs.term->obj.lambda);

    Location dummy;

    CloneAst(&body, lambda->body);
    CloneAst(&value, rhs.term); // pass by-value
    Judgement type = Getype(rhs.term, env);

    if (type.success == false)
    {
      result = type;
      return result;
    }

    if (lambda->curryed == true)
    {
      // if this lambda object is curryed, then we do not have enough arguments
      // held within this tree node to perfrom the full substitution.
      result.success = true;
      result.term    = CreateAstPartiallyAppliedLambda(body, lambda->arg_id, lambda->scope, value, type.term, &dummy);
    }
    else
    {
      // this is a single argument procedure, being applied within a
      // single argument application term.
      SymbolTable* lastsymbols = env->symbols;
      ScopeSet     lastscope   = env->scope;

      env->symbols = lambda->symbols;
      env->scope   = lambda->scope;
      // bind the instances of the name appearing throughout
      // the body to the argument that was passed in.
      Substitute(body, &body, lambda->arg_id, value, env);

      // evaluate the newly substituted body of the lambda.
      result = Evaluate(body, env);

      env->symbols = lastsymbols;
      env->scope   = lastscope;
    }
  }
  else if (is_partial_application(lhs.term))
  {
    // we are currently either building up values
    // to apply within the lambda term, or we are
    // passing in the final argument and performing
    // all of the built up substitutions.
    rhs = Evaluate(app->rhs, env);

    if (rhs.success == true)
    {
      CloneAst(&(value), rhs.term);
      result = ApplyArgument(lhs.term, value, env);
    }
    else
    {
      result = rhs;
    }
  }
  else
  {
    char *c0 = "Cannot apply non Lambda Object [";
    char *c1 = "]";
    char *o  = ToStringAst(lhs.term);
    char *t0 = appendstr(c0, o);
    result.success = false;
    result.error.loc = *GetAstLocation(lhs.term);
    result.error.dsc = appendstr(t0, c1);
    free(o);
    free(t0);
    return result; // leaks lhs
  }


  return result;
}

bool AppearsFreeApplication(Application* app, InternedString id)
{
  bool l = AppearsFree(app->lhs, id);
  bool r = AppearsFree(app->rhs, id);
  return l || r;
}

void RenameBindingApplication(Application* app, InternedString target, InternedString replacement)
{
  RenameBinding(app->lhs, target, replacement);
  RenameBinding(app->rhs, target, replacement);
}

void SubstituteApplication(Application* app, Ast** target, InternedString id, Ast* value, struct Environment* env)
{
  Substitute(app->lhs, &(app->lhs), id, value, env);
  Substitute(app->rhs, &(app->rhs), id, value, env);
}
