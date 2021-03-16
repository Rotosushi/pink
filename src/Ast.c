#include <stdlib.h>
#include <string.h>

#include "Utilities.h"
#include "ScopeSet.h"
#include "Judgement.h"
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

Ast* CreateAstVariable(InternedString id, ScopeSet scope, Location* loc)
{
  Ast* ast  = (Ast*)malloc(sizeof(Ast));
  ast->kind = A_VAR;
  InitializeVariable(&(ast->var), id, scope, loc);
  return ast;
}

Ast* CreateAstApplication(Ast* left, Ast* right, Location* loc)
{
  Ast* ast  = (Ast*)malloc(sizeof(Ast));
  ast->kind = A_APP;
  InitializeApplication(&(ast->app), left, right, loc);
  return ast;
}

Ast* CreateAstAssignment(Ast* left, Ast* right, Location* loc)
{
  Ast* ast  = (Ast*)malloc(sizeof(Ast));
  ast->kind = A_ASS;
  InitializeAssignment(&(ast->ass), left , right, loc);
  return ast;
}

Ast* CreateAstBind(InternedString id, Ast* right, Location* loc)
{
  Ast* ast  = (Ast*)malloc(sizeof(Ast));
  ast->kind = A_BND;
  InitializeBind(&(ast->bnd), id, right, loc);
  return ast;
}

Ast* CreateAstBinop(InternedString op, Ast* left, Ast* right, Location* loc)
{
  Ast* ast  = (Ast*)malloc(sizeof(Ast));
  ast->kind = A_BOP;
  InitializeBinop(&(ast->bop), op, left, right, loc);
  return ast;
}

Ast* CreateAstUnop(InternedString op, Ast* right, Location* loc)
{
  Ast* ast  = (Ast*)malloc(sizeof(Ast));
  ast->kind = A_UOP;
  InitializeUnop(&(ast->uop), op, right, loc);
  return ast;
}

Ast* CreateAstConditional(Ast* condition, Ast* first, Ast* second, Location* loc)
{
  Ast* ast  = (Ast*)malloc(sizeof(Ast));
  ast->kind = A_CND;
  InitializeConditional(&(ast->cnd), condition, first, second, loc);
  return ast;
}

Ast* CreateAstIteration(Ast* condition, Ast* body, Location* loc)
{
  Ast* ast  = (Ast*)malloc(sizeof(Ast));
  ast->kind = A_ITR;
  InitializeIteration(&(ast->itr), condition, body, loc);
  return ast;
}

Ast* CreateAstNil(Location* loc)
{
  Ast* ast      = (Ast*)malloc(sizeof(Ast));
  ast->kind     = A_OBJ;
  ast->obj.kind = O_NIL;
  InitializeNil(&(ast->obj.nil), loc);
  return ast;
}

Ast* CreateAstInteger(int value, Location* loc)
{
  Ast* ast      = (Ast*)malloc(sizeof(Ast));
  ast->kind     = A_OBJ;
  ast->obj.kind = O_INT;
  InitializeInteger(&(ast->obj.integer), value, loc);
  return ast;
}

Ast* CreateAstBoolean(bool value, Location* loc)
{
  Ast* ast      = (Ast*)malloc(sizeof(Ast));
  ast->kind     = A_OBJ;
  ast->obj.kind = O_BOOL;
  InitializeBoolean(&(ast->obj.boolean), value, loc);
  return ast;
}

Ast* CreateAstLambda(InternedString arg_id, Ast* arg_type, Ast* body, SymbolTable* symbols, ScopeSet scope, Location* loc)
{
  Ast* ast      = (Ast*)malloc(sizeof(Ast));
  ast->kind     = A_OBJ;
  ast->obj.kind = O_LAMB;
  InitializeLambda(&(ast->obj.lambda), arg_id, arg_type, body, symbols, scope, loc);
  return ast;
}

Ast* CreateAstType(Type* type, Location* loc)
{
  Ast* ast      = (Ast*)malloc(sizeof(Ast));
  ast->kind     = A_OBJ;
  ast->obj.kind = O_TYPE;
  InitializeTypeLiteral(&(ast->obj.type), type, loc);
  return ast;
}

Judgement Assign(Ast* dest, Ast* source)
{
  if (!dest || !source)
    FatalError("Bad Term Pointer(s).", __FILE__, __LINE__);
  Judgement result;
  if (dest->kind != A_OBJ)
  {
    result.success   = false;
    result.error.loc = *GetAstLocation(dest);
    result.error.dsc = dupstr("Left term is not an Object! Cannot assign");
    return result;
  }

  if (source->kind != A_OBJ)
  {
    result.success   = false;
    result.error.loc = *GetAstLocation(source);
    result.error.dsc = dupstr("Right term is not an Object! Cannot assign");
    return result;
  }

  result = AssignObject(&(dest->obj), &(source->obj));
  return result;
}

Judgement Equals(Ast* left, Ast* right)
{
  if (!left)
    FatalError("Bad Left Term Pointer.", __FILE__, __LINE__);

  if (!right)
    FatalError("Bad right term pointer.", __FILE__, __LINE__);

  Judgement result;

  if (left->kind != A_OBJ)
  {
    result.success   = false;
    result.error.loc = *GetAstLocation(left);
    result.error.dsc = dupstr("Left term is not an Object! Cannot compare equality");
    return result;
  }

  if (right->kind != A_OBJ)
  {
    result.success   = false;
    result.error.loc = *GetAstLocation(right);
    result.error.dsc = dupstr("Right term is not an Object! Cannot compare equality");
    return result;
  }

  result = EqualsObject(&(left->obj), &(right->obj));
  return result;
}

Location* GetAstLocation(Ast* ast)
{
  switch(ast->kind)
  {
    case A_VAR:
      return &(ast->var.loc);
    case A_APP:
      return &(ast->app.loc);
    case A_ASS:
      return &(ast->ass.loc);
    case A_BND:
      return &(ast->bnd.loc);
    case A_BOP:
      return &(ast->bop.loc);
    case A_UOP:
      return &(ast->uop.loc);
    case A_CND:
      return &(ast->cnd.loc);
    case A_ITR:
      return &(ast->itr.loc);
    case A_OBJ:
      return GetObjLocation(&(ast->obj));
    default:
      FatalError("Bad Ast Kind", __FILE__, __LINE__);
  }
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
Ast* CloneAst(Ast** destination, Ast* source)
{
  if ((*destination) == NULL)
    (*destination) = (Ast*)malloc(sizeof(Ast));
  (*destination)->kind = source->kind;

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

    default:
      FatalError("Bad Ast Kind", __FILE__, __LINE__);
      break;
  }
  return (*destination);
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

    default:
      FatalError("Bad Ast Kind", __FILE__, __LINE__);
      break;
  }
  return result;
}

// now you may say, why add in another function call
// when you already know exactly what needs doing?
// and for two reasons
// A) it keeps this file from being >3000 lines
// B) the compiler can still inline each procedure if it wants.
Judgement Getype(Ast* term, struct Environment* env)
{
  Judgement result;
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

    default:
      FatalError("Bad Ast Kind", __FILE__, __LINE__);
  }
  return result;
}

Judgement Evaluate(Ast* ast, Environment* env)
{
  Judgement result;

  switch(ast->kind)
  {
    case A_VAR:
      result = EvaluateVariable(&(ast->var), env);
      break;
    case A_APP:
      result = EvaluateApplication(&(ast->app), env);
      break;
    case A_ASS:
      result = EvaluateAssignment(&(ast->ass), env);
      break;
    case A_BND:
      result = EvaluateBind(&(ast->bnd), env);
      break;
    case A_BOP:
      result = EvaluateBinop(&(ast->bop), env);
      break;
    case A_UOP:
      result = EvaluateUnop(&(ast->uop), env);
      break;
    case A_CND:
      result = EvaluateConditional(&(ast->cnd), env);
      break;
    case A_ITR:
      result = EvaluateIteration(&(ast->itr), env);
      break;
    case A_OBJ:
      result.success = true;
      result.term    = ast;
      break;
    default:
      FatalError("Bas Ast Kind", __FILE__, __LINE__);
  }

  return result;
}


bool AppearsFree(Ast* term, InternedString id)
{
  bool result = false;
  switch (term->kind)
  {
    case A_VAR:
      result = AppearsFreeVariable(&(term->var), id);
      break;
    case A_APP:
      result = AppearsFreeApplication(&(term->app), id);
      break;
    case A_ASS:
      result = AppearsFreeAssignment(&(term->ass), id);
      break;
    case A_BND:
      result = AppearsFreeBind(&(term->bnd), id);
      break;
    case A_BOP:
      result = AppearsFreeBinop(&(term->bop), id);
      break;
    case A_UOP:
      result = AppearsFreeUnop(&(term->uop), id);
      break;
    case A_CND:
      result = AppearsFreeConditional(&(term->cnd), id);
      break;
    case A_ITR:
      result = AppearsFreeIteration(&(term->itr), id);
      break;
    case A_OBJ:
      result = AppearsFreeObject(&(term->obj), id);
      break;
    default:
      FatalError("Bad Ast Kind", __FILE__, __LINE__);
  }
  return result;
}

void RenameBinding(Ast* term, InternedString target, InternedString replacement)
{
  switch(term->kind)
  {
    case A_VAR:
      RenameBindingVariable(&(term->var), target, replacement);
      break;
    case A_APP:
      RenameBindingApplication(&(term->app), target, replacement);
      break;
    case A_ASS:
      RenameBindingAssignment(&(term->ass), target, replacement);
      break;
    case A_BND:
      RenameBindingBind(&(term->bnd), target, replacement);
      break;
    case A_BOP:
      RenameBindingBinop(&(term->bop), target, replacement);
      break;
    case A_UOP:
      RenameBindingUnop(&(term->uop), target, replacement);
      break;
    case A_CND:
      RenameBindingConditional(&(term->cnd), target, replacement);
      break;
    case A_ITR:
      RenameBindingIteration(&(term->itr), target, replacement);
      break;
    case A_OBJ:
      RenameBindingObject(&(term->obj), target, replacement);
      break;
    default:
      FatalError("Bad Ast Kind", __FILE__, __LINE__);
  }
}

// so, interestingly, only calls to Ast nodes that turn out
// to be Variables ever need the Target pointer.
void Substitute(Ast* term, Ast** target, InternedString id, Ast* value, Environment* env)
{
  switch(term->kind)
  {
    case A_VAR:
      SubstituteVariable(&(term->var), target, id, value, env);
      break;
    case A_APP:
      SubstituteApplication(&(term->app), target, id, value, env);
      break;
    case A_ASS:
      SubstituteAssignment(&(term->ass), target, id, value, env);
      break;
    case A_BOP:
      SubstituteBinop(&(term->bop), target, id, value, env);
      break;
    case A_UOP:
      SubstituteUnop(&(term->uop), target, id, value, env);
      break;
    case A_CND:
      SubstituteConditional(&(term->cnd), target, id, value, env);
      break;
    case A_ITR:
      SubstituteIteration(&(term->itr), target, id, value, env);
      break;
    case A_OBJ:
      SubstituteObject(&(term->obj), target, id, value, env);
      break;
    default:
      FatalError("Bad Ast Kind", __FILE__, __LINE__);
  }
}
