#include <stdlib.h>
#include <string.h>

#include "Ast.h"
#include "Utilities.h"
#include "Location.h"
#include "Judgement.h"
#include "Environment.h"
#include "Assignment.h"

void InitializeAssignment(Assignment* ass, struct Ast* left, struct Ast* right, Location* loc)
{
  ass->loc = *loc;
  ass->lhs = left;
  ass->rhs = right;
}

void DestroyAssignment(Assignment* ass)
{
  DestroyAst(ass->lhs);
  DestroyAst(ass->rhs);
}

void CloneAssignment(Assignment* dest, Assignment* source)
{
  dest->loc = source->loc;
  CloneAst(&(dest->lhs), source->lhs);
  CloneAst(&(dest->rhs), source->rhs);
}

char* ToStringAssignment(Assignment* ass)
{
  char *result, *larrow, *left, *right, *ctx;
  larrow = " <- "; // 4 chars here, plus the null terminator makes 5
  left   = ToStringAst(ass->lhs);
  right  = ToStringAst(ass->rhs);

  int sz = strlen(left) + strlen(right) + 5;
  result = (char*)calloc(sz, sizeof(char));

  strkat(result, left,   &ctx);
  strkat(result, larrow, &ctx);
  strkat(result, right,  &ctx);
  free(left);
  free(right);
  return result;
}

/*
   ENV |- lhs : T, rhs : T
------------------------------
   ENV |- lhs '<-' rhs : T
*/
Judgement GetypeAssignment(Assignment* ass, Environment* env)
{
  Judgement result;

  Judgement lhsjdgmt = Getype(ass->lhs, env); // dynamic result

  if (lhsjdgmt.success == true)
  {
    Judgement rhsjdgmt = Getype(ass->rhs, env); // free me

    if (rhsjdgmt.success == true)
    {
      Judgement eqjdgmt = Equals(lhsjdgmt.term, rhsjdgmt.term); // on success eqjdgmt.term == NULL
      if (eqjdgmt.success == true)
      {
        if (eqjdgmt.term->obj.boolean.value == true)
          result = lhsjdgmt;
        else
        {
          char* t1  = "Type mismatch between lhs:[";
          char* t2  = "] and rhs:[";
          char* t3  = "]";
          char* lhs = ToStringAst(lhsjdgmt.term);
          char* rhs = ToStringAst(rhsjdgmt.term);
          // t1 + lhs + t2 + rhs + t3
          char* i0     = appendstr(t1, lhs);
          char* i1     = appendstr(i0, t2);
          char* i2     = appendstr(i1, rhs);
          char* errtxt = appendstr(i2, t3);
          result.success   = false;
          result.error.dsc = errtxt;
          result.error.loc = ass->loc;
          free(lhs);
          free(rhs);
          free(i0);
          free(i1);
          free(i2);
        }
      }
      else
      {
        result = eqjdgmt;
      }
    }
    else
      result = rhsjdgmt;
  }
  else
    result = lhsjdgmt;

  return result;
}

Judgement EvaluateAssignment(Assignment* ass, Environment* env)
{
  Judgement result, lhs, rhs;

  // evaluate the assignment

  lhs = Evaluate(ass->lhs, env);

  if (lhs.success == true)
  {
    rhs = Evaluate(ass->rhs, env);

    if (rhs.success == true)
    {
      // after evaluation, we have two objects.
      // now, previously i could do pointer assignment
      // here, which is easy and fast. however now.
      // I have to come up with a new solution.
      // the solution is essentially the overload set of
      // '=' which is valid for expressions with the type
      // TypeLiteral '=' TypeLiteral
      // Nil '=' Nil
      // Int '=' Int
      // Bool '=' Bool
      //
      result = AssignObject(&(lhs.term->obj), &(rhs.term->obj));

      // we want to preserve the error
      // should it exist, otherwise
      // AssignObject returned an empty
      // term.
      if (result.success == true)
      {
        result = rhs;
      }
    }
    else
      result = rhs;
  }
  else
    result = lhs;

  return result;
}

bool AppearsFreeAssignment(Assignment* ass, InternedString id)
{
  bool l = AppearsFree(ass->lhs, id);
  bool r = AppearsFree(ass->rhs, id);
  return l || r;
}

void RenameBindingAssignment(Assignment* ass, InternedString target, InternedString replacement)
{
  RenameBinding(ass->lhs, target, replacement);
  RenameBinding(ass->rhs, target, replacement);
}

void SubstituteAssignment(Assignment* ass, Ast** target, InternedString id, Ast* value, Environment* env)
{
  Substitute(ass->lhs, &(ass->lhs), id, value, env);
  Substitute(ass->rhs, &(ass->rhs), id, value, env);
}
