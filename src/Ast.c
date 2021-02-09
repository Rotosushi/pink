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
#include "Object.h"
#include "Type.h"
#include "Ast.h"

Ast* CreateVariable(InternedString id, Location* loc)
{
  Ast* ast  = (Ast*)malloc(sizeof(Ast));
  ast->kind = A_VAR;
  ast->loc  = *loc;
  return ast;
}

Ast* CreateApplication(Ast* left, Ast* right, Location* loc);
Ast* CreateAssignment(Ast* left, Ast* right, Location* loc);
Ast* CreateBind(InternedString id, Ast* right, Location* loc);
Ast* CreateBinop(InternedString op, Ast* left, Ast* right, Location* loc);
Ast* CreateUnop(InternedString op, Ast* right, Location* loc);
Ast* CreateConditional(Ast* condition, Ast* first_alternative, Ast* second_alternative, Location* loc);
Ast* CreateIteration(Ast* condition, Ast* body, Location* loc);
Ast* CreateNil(Location* loc);
Ast* CreateInteger(int  value, Location* loc);
Ast* CreateBoolean(bool value, Location* loc);
Ast* CreateLambda(InternedString arg_id, Type* arg_type, Ast* body, Location* loc);
Ast* CreateType(Type* type, Location* loc);
Ast* CreateScalarType(ScalarKind kind, struct TypeInterner* interned_types, Location* loc);
Ast* CreateProcType(Type* left, Type* right, struct TypeInterner* interned_types, Location* loc);

// this shall deallocate all memory associated
// with term, such that after this procedure
// returns term is null.
void DestroyAst(Ast* term)
{
  switch(term->kind)
  {
    case A_VAR:
      DestroyVariable(&(term->var));
      break;

    case A_APP:
      DestroyApplication(&(term->app));
      break;

    case A_ASS:
      DestroyAssignment(&(term->ass));
      break;

    case A_BND:
      DestroyBind(&(term->bnd));
      break;

    case A_BOP:
      DestroyBinop(&(term->bop));
      break;

    case A_UOP:
      DestroyUnop(&(term->uop));
      break;

    case A_CND:
      DestroyConditional(&(term->cnd));
      break;

    case A_ITR:
      DestroyIteration(&(term->itr));
      break;

    case A_OBJ:
      DestroyObject(&(term->obj));
      break;

    case A_TYP:
      DestroyType(&(term->typ));
      break;

    default:
      FatalError("Bad Ast Kind", __FILE__, __LINE__);
      break;
  }
  free(term);
  term = NULL;
}

// constructs a new tree exactly the same as the
// passed tree.
void CloneAst(Ast** destination, Ast* source);
{
  if ((*destination) == NULL)
    (*destination) = (Ast*)malloc(sizeof(Ast));
  (*destination)->kind = source->kind;
  (*destination)->loc  = source->loc;

  switch(source->kind)
  {
    case A_VAR:
      CloneVariable(&((*destination)->var), &(source->var));
      break;

    case A_APP:
      CloneApplication(&((*destination)->app), &(source->app));
      break;

    case A_ASS:
      CloneAssignment(&((*destination)->ass), &(source->ass));
      break;

    case A_BND:
      CloneBind(&((*destination)->bnd), &(source->bnd));
      break;

    case A_BOP:
      CloneBinop(&((*destination)->bop), &(source->bop));
      break;

    case A_UOP:
      CloneUnop(&((*destination)->uop), &(source->uop));
      break;

    case A_CND:
      CloneConditional(&((*destination)->cnd), &(source->cnd));
      break;

    case A_ITR:
      CloneIteration(&((*destination)->itr), &(source->itr));
      break;

    case A_OBJ:
      CloneObject(&((*destination)->obj), &(source->obj));
      break;

    case A_TYP:
      CloneType(&((*destination)->typ), &(source->typ));

    default:
      FatalError("Bad Ast Kind", __FILE__, __LINE__);
      break;
  }
  return result;
}

// constructs a string that represents the passed
// in term.
char* ToStringAst(Ast* term)
{
  char* result;
  switch(term->kind)
  {
    case A_VAR:
      result = ToStringVariable(&(term->var));
      break;

    case A_APP:
      result = ToStringApplication(&(term->app));
      break;

    case A_ASS:
      result = ToStringAssignment(&(term->ass));
      break;

    case A_BND:
      result = ToStringBind(&(term->bnd));
      break;

    case A_BOP:
      result = ToStringBinop(&(term->bop));
      break;

    case A_UOP:
      result = ToStringUnop(&(term->uop));
      break;

    case A_CND:
      result = ToStringConditional(&(term->cnd));
      break;

    case A_ITR:
      result = ToStringIteration(&(term->itr));
      break;

    case A_OBJ:
      result = ToStringObject(&(term->obj));
      break;

    case A_TYP:
      result = ToStringType(&(term->typ));
      break;

    default:
      FatalError("Bad Ast Kind", __FILE__, __LINE__);
      break;
  }
  return result;
}

TypeJudgement Getype(Ast* term, struct Environment* env)
{
  TypeJudgement result;
  switch(term->kind)
  {
    case A_VAR:
      result = GetypeVariable(&(term->var), env);
      break;

    case A_APP:
      result = GetypeApplication(&(term->app), env);
      break;

    case A_ASS:
      result = GetypeAssignment(&(term->ass), env);
      break;

    case A_BND:
      result = GetypeBind(&(term->bnd), env);
      break;

    case A_BOP:
      result = GetypeBinop(&(term->bop), env);
      break;

    case A_UOP:
      result = GetypeUnop(&(term->uop), env);
      break;

    case A_CND:
      result = GetypeConditional(&(term->cnd), env);
      break;

    case A_ITR:
      result = GetypeIteration(&(term->itr), env);
      break;

    case A_OBJ:
      result = GetypeObject(&(term->obj), env);
      break;

    case A_TYP:
      result.success = true;
      result.type    = term->typ;
      break;

    default
      FatalError("Bad Ast Kind", __FILE__, __LINE__);
  }
  return result;
}
