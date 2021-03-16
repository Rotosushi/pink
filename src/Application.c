#include <stdlib.h>
#include <string.h>

#include "Ast.h"
#include "Utilities.h"
#include "Location.h"
#include "Judgement.h"
#include "Environment.h"
#include "Application.h"

void InitializeApplication(Application* app, struct Ast* left, struct Ast* right, Location* loc)
{
  app->loc = *loc;
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
          // why does it segfault when implemented as
          // result.term = CloneAst(&(result.term), rhsjdgmt.term); ?
          Ast* ast = NULL;
          result.success = true;
          CloneAst(&(ast), rhsjdgmt.term);
          result.term    = ast;
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
  if ((term->kind == A_OBJ) && (term->obj.kind == O_LAMB))
    result = true;
  return result;
}


Judgement EvaluateApplication(Application* app, Environment* env)
{
  Judgement result, lhs, rhs;

  // evaluate the application
  // evaluate the lhs down to a lambda literal
  // evaluate the rhs to an object.
  // make a copy of the lambda's body for substitution.
  // Substitute the object in for the argument name
  // within the body of the lambda.
  // return the newly substituted body term as the result.

  lhs = Evaluate(app->lhs, env);

  if (lhs.success == false)
  {
    result = lhs;
    return result;
  }

  if (!is_lambda_literal(lhs.term))
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

  rhs = Evaluate(app->rhs, env);

  if (rhs.success == false)
  {
    result = rhs;
    return result;
  }

  // I think we -have- to copy the procedures
  // body, and perform the substitution upon
  // that. this is what prevents us from modifying
  // the stored copy within the SymbolTable.
  // because the variable binding returns
  // the pointer to the term within the
  // symboltable upon Lookup, were we to
  // substitute on that term we would modify
  // the held copy, permanently changing the
  // term within the symboltable. this is
  // a mistake, and doesn't allow us to call
  // the procedure with more than one set of
  // actual arguments.
  Lambda* lambda = &(lhs.term->obj.lambda);
  Ast* body = NULL, *value = NULL;
  CloneAst(&body, lambda->body);
  CloneAst(&value, rhs.term); // pass by-value

  // bind the instances of the name appearing throughout
  // the body to the argument that was passed in.
  Substitute(body, &body, lambda->arg_id, value, env);

  // evaluate the newly substituted body of the lambda.
  result = Evaluate(body, env);
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
