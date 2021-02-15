#include <stdlib.h>
#include <string.h>

#include "TypeJudgement.h"
#include "Environment.h"
#include "SymbolTable.h"
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
  InitializeVariable(&(ast->var), id);
  return ast;
}

Ast* CreateApplication(Ast* left, Ast* right, Location* loc)
{
  Ast* ast  = (Ast*)malloc(sizeof(Ast));
  ast->kind = A_APP;
  ast->loc  = *loc;
  InitializeApplication(&(ast->app), left, right);
  return ast;
}

Ast* CreateAssignment(Ast* left, Ast* right, Location* loc)
{
  Ast* ast  = (Ast*)malloc(sizeof(Ast));
  ast->kind = A_ASS;
  ast->loc  = *loc;
  InitializeAssignment(&(ast->ass), left , right);
  return ast;
}

Ast* CreateBind(InternedString id, Ast* right, Location* loc)
{
  Ast* ast  = (Ast*)malloc(sizeof(Ast));
  ast->kind = A_BND;
  ast->loc  = *loc;
  InitializeBind(&(ast->bnd), id, right);
  return ast;
}

Ast* CreateBinop(InternedString op, Ast* left, Ast* right, Location* loc)
{
  Ast* ast  = (Ast*)malloc(sizeof(Ast));
  ast->kind = A_BOP;
  ast->loc  = *loc;
  InitializeBinop(&(ast->bop), op, left, right);
  return ast;
}

Ast* CreateUnop(InternedString op, Ast* right, Location* loc)
{
  Ast* ast  = (Ast*)malloc(sizeof(Ast));
  ast->kind = A_UOP;
  ast->loc  = *loc;
  InitializeUnop(&(ast->uop), op, right);
  return ast;
}

Ast* CreateConditional(Ast* condition, Ast* first, Ast* second, Location* loc)
{
  Ast* ast  = (Ast*)malloc(sizeof(Ast));
  ast->kind = A_CND;
  ast->loc  = *loc;
  InitializeConditional(&(ast->cnd), condition, first, second);
  return ast;
}

Ast* CreateIteration(Ast* condition, Ast* body, Location* loc)
{
  Ast* ast  = (Ast*)malloc(sizeof(Ast));
  ast->kind = A_ITR;
  ast->loc  = *loc;
  InitializeIteration(&(ast->itr), condition, body);
  return ast;
}

Ast* CreateNil(Location* loc)
{
  Ast* ast           = (Ast*)malloc(sizeof(Ast));
  ast->kind          = A_OBJ;
  ast->loc           = *loc;
  ast->obj.kind      = O_NIL;
  InitializeNil(&(ast->obj.nil));
  return ast;
}

Ast* CreateInteger(int value, Location* loc)
{
  Ast* ast               = (Ast*)malloc(sizeof(Ast));
  ast->kind              = A_OBJ;
  ast->loc               = *loc;
  ast->obj.kind          = O_INT;
  InitializeInteger(&(ast->obj.integer), value);
  return ast;
}

Ast* CreateBoolean(bool value, Location* loc)
{
  Ast* ast               = (Ast*)malloc(sizeof(Ast));
  ast->kind              = A_OBJ;
  ast->loc               = *loc;
  ast->obj.kind          = O_BOOL;
  InitializeBoolean(&(ast->obj.boolean), value);
  return ast;
}

Ast* CreateLambda(InternedString arg_id, Type* arg_type, Ast* body, SymbolTable* scope, Location* loc)
{
  Ast* ast = (Ast*)malloc(sizeof(Ast));
  ast->kind = A_OBJ;
  ast->loc  = *loc;
  ast->obj.kind = O_LAMB;
  InitializeLambda(&(ast->obj.lambda), arg_id, arg_type, body, scope);
  return ast;
}

Ast* CreateType(Type* type, Location* loc)
{
  Ast* ast  = (Ast*)malloc(sizeof(Ast));
  ast->kind = A_TYP;
  ast->loc  = *loc;
  ast->typ  = type;
  return ast;
}

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
      // the type interner handles the memory representing types
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
// and if we imagine passing the address of a
// Ast allocated on the stack, we encounter
// a non NULL destination ptr. so the
// code still works! :)
void CloneAst(Ast** destination, Ast* source)
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
      (*destination)->typ = source->typ;

    default:
      FatalError("Bad Ast Kind", __FILE__, __LINE__);
      break;
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
      result = ToStringType(term->typ);
      break;

    default:
      FatalError("Bad Ast Kind", __FILE__, __LINE__);
      break;
  }
  return result;
}

// now you may say, why add in another function call
// when you already known exactly what needs doing?
// and for two reasons
// A) it keeps this file from being >3000 lines
// B) the compiler can still inline each procedure if it wants.
TypeJudgement Getype(Ast* term, struct Environment* env)
{
  TypeJudgement result;
  switch(term->kind)
  {
    case A_VAR:
      result = GetypeVariable(term, env);
      break;

    case A_APP:
      result = GetypeApplication(term, env);
      break;

    case A_ASS:
      result = GetypeAssignment(term, env);
      break;

    case A_BND:
      result = GetypeBind(term, env);
      break;

    case A_BOP:
      result = GetypeBinop(term, env);
      break;

    case A_UOP:
      result = GetypeUnop(term, env);
      break;

    case A_CND:
      result = GetypeConditional(term, env);
      break;

    case A_ITR:
      result = GetypeIteration(term, env);
      break;

    case A_OBJ:
      result = GetypeObject(term, env);
      break;

    case A_TYP:
      result.success = true;
      result.type    = term->typ;
      break;

    default:
      FatalError("Bad Ast Kind", __FILE__, __LINE__);
  }
  return result;
}
