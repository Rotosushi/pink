#include <stdlib.h>
#include <string.h>

#include "Variable.h"
#include "Application.h"
#include "Assignment.h"
#include "Bind.h"
#include "Binop.h"
#include "Unop.h"
#include "Conditional.h"
#include "Iteration.h"
#include "Entity.h"
#include "Ast.h"

// this shall deallocate all memory associated
// with term, such that after this procedure
// returns term is null.
void DestroyAst(Ast* term)
{
  switch(term->kind)
  {
    case A_VAR:
      DestroyVariable(term->var);
      break;

    case A_APP:
      DestroyApplication(term->app);
      break;

    case A_ASS:
      DestroyAssignment(term->ass);
      break;

    case A_BND:
      DestroyBind(term->bnd);
      break;

    case A_BOP:
      DestroyBinop(term->bop);
      break;

    case A_UOP:
      DestroyUnop(term->uop);
      break;

    case A_CND:
      DestroyConditional(term->cnd);
      break;

    case A_ITR:
      DestroyIteration(term->itr);
      break;

    case A_ENT:
      DestroyEntity(term->ent);
      break;

    default:
      // report the fatal error
  }
  free(term);
  term = NULL;
}

// constructs a new tree exaclty the same as the
// passed tree.
Ast* CloneAst(Ast* term)
{
  Ast* result  = (Ast*)malloc(sizeof(Ast));
  result->kind = term->kind;
  result->loc  = term->loc;

  switch(result->kind)
  {
    case A_VAR:
      result->var = CloneVariable(term->var);
      break;

    case A_APP:
      result->app = CloneApplication(term->app);
      break;

    case A_ASS:
      result->ass = CloneAssignment(term->ass);
      break;

    case A_BND:
      result->bnd = CloneBind(term->bnd);
      break;

    case A_BOP:
      result->bop = CloneBinop(term->bop);
      break;

    case A_UOP:
      result->uop = CloneUnop(term->uop);
      break;

    case A_CND:
      result->cnd = CloneConditional(term->cnd);
      break;

    case A_ITR:
      result->itr = CloneIteration(term->itr);
      break;

    case A_ENT:
      result->ent = CloneEntity(term->ent);
      break;

    default:
      // report the fatal error
  }
}

// constructs a string that represents the passed
// in term.
char* ToStringAst(Ast* term)
{
  char* result;
  switch(term->kind)
  {
    case A_VAR:
      result = ToStringVariable(term->var);
      break;

    case A_APP:
      result = ToStringApplication(term->app);
      break;

    case A_ASS:
      result = ToStringAssignment(term->ass);
      break;

    case A_BND:
      result = ToStringBind(term->bnd);
      break;

    case A_BOP:
      result = ToStringBinop(term->bop);
      break;

    case A_UOP:
      result = ToStringUnop(term->uop);
      break;

    case A_CND:
      result = ToStringConditional(term->cnd);
      break;

    case A_ITR:
      result = ToStringIteration(term->itr);
      break;

    case A_ENT:
      result = ToStringEntity(term->ent);
      break;

    default:
      // report the fatal error
  }
  return result;
}
