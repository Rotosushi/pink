#include <stdlib.h>
#include <string.h>

#include "Utilities.hpp"
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

Ast* CreateAstApplication(Ast* left, Ast* right, int num_args, Location* loc)
{
  Ast* ast  = (Ast*)malloc(sizeof(Ast));
  ast->kind = A_APP;
  InitializeApplication(&(ast->app), left, right, num_args, loc);
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
  ast->obj.kind = O_LAMBDA;
  InitializeLambda(&(ast->obj.lambda), arg_id, arg_type, body, symbols, scope, loc);
  return ast;
}

Ast* CreateAstPartiallyAppliedLambda(Ast* rest, InternedString arg_name, ScopeSet arg_scope, Ast* arg_value, Ast* arg_type, Location* loc)
{
  Ast* ast = (Ast*)malloc(sizeof(Ast));
  ast->kind = A_OBJ;
  ast->obj.kind = O_PARAPP;
  InitializePartiallyAppliedLambda(&(ast->obj.parapp), rest, arg_name, arg_scope, arg_value, arg_type, loc);
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

Type* GetTypePtrFromLiteral(Ast* ast)
{
  Type* result = NULL;
  if (ast->kind == A_OBJ)
  {
    if (ast->obj.kind == O_TYPE)
    {
      result = ast->obj.type.literal;
    }
  }
  return result;
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
//     and oh boy would this file be huge if every
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
  /*
  why doesn't substitute's signature look like?
  void Substitute(Ast** target, InternedString id, Ast* value, Environment* env)

  well, this is actually interesting, because it has to do
  with type specialization/reduction/shortening along what i
  think is the Object boundary that OOP languages observe.

  to cut directly to the point, substitution is performed
  via pointer assignment. this has the effect that we need
  a reference to the parent pointer, specifically, the cell
  of memory held within the ast structure 'above' the
  variable which is being substituted. this allows the
  procedure to replace the pointer in the cell with the
  pointer to the value, thus, substituting a variable for
  a value.
  now, since i am thinking
  about procedure overloading which works atop C procedure
  calls, i am treating Substitute and SubstituteBind and such
  as an overload set. now, in each other procedure in this
  program (getype, evaluate, etc) the type specialization
  that is occurring to differenciate between instances of
  this overload set, essentially:

    Judgement Getype(Ast* term, Environment* env);
    Judgement GetypeBind(Bind* bind, Environment* env);
    ...

  there is a difference in the type of the first argument
  alongside the difference in name (that is required by the
  C standard.) in a programming language with support for
  overloading procedures by argument type we could define
  Getype like:

    Judgement Getype(Variable* term, Environment* env);
    Judgement Getype(Bind* bind, Environment* env);
    ...

  and then within the mutually recursive definition of
  Getype, the programmer always calls Getype passing in
  an Ast, because that is how the Ast is constructed.
  however the programmer only defines bodies for the
  members of the union type that Ast describes, and
  thus the programmer doesn't have to write the switch
  statement which dispatches to the correct procedure
  given which member is currently occupying the node
  being pointed to by the Ast*. the compiler can infer
  the switch by which types are used as argument for
  the overload set. if a type is defined and then used
  as an argument, then any call of the procedure passing
  in a variable of said type can be dispatched at compile
  time to the correct body. and type defined as a sum of
  types or as an alias of another existing type can be
  used as a formal argument, and then actual argument type
  can be used to dispatch to the correct body at compile time.
  any type which is defined as a set of alternative types
  can be passed into a procedure correctly, only if there is
  a member of the procedures overload set for each type present
  within the alternative type set. and the dispatch routine
  which takes the set of alternates type can be implemented
  as a switch over the runtime type of the member, dispatching
  to each of the correct members of the overload set.
  (recursion can be handled by a second dispatch routine,
   i.e. if one of the alternatives is itself a sum of
    alternatives, the disptach routine which strips off
    the first set, dispatches to a second routine which
    disptaches over which member is currently active in
    that set. this of course means that each alternative in
    every member of the set is represented flatly from the
    programmers perspective. they simply define a body for
    each alternative that is not a union, and the compiler
    builds disptach routines for each union type, recursively
    peeling away unions until the actual active sum type is
    represented.)

  and now we reach the rub, you see, Substitution performs it's
  work on Ast*'s. and we just defined a method of overloading
  that never deals in the actual type of the set of alternatives
  type. the programmer only defines bodies accepting the
  member types. this means that substituion itself could not
  be defined in the straightforward way.
  we would need to define it differently.

  one thing that is absolutely critical is the existance of
  an Ast**, i.e. a reference to the cell of memory which needs
  to be overwritten in the argument list. otherwise how will that
  cell of memory be reachable while we are traversing the node
  below. additionally, while we only ever actually substitute
  on a node that happens to contain a variable, we could find
  that variable as a leaf to one of any of the other nodes.

  this is the impetus behind the argument list of substitution.
  one of the ideas i had would be to allow the programmer to
  explicitly declare a body taking a pointer to a union type
  directly, meaning wrapper procedures could be written by
  taking the pointer, and then the runtime dispatch can be
  invoked by unwraping the pointer to it's union directly
  and passing that into the procedure by-value.
  this would allow a reference to the cell of memory containing
  the reference to the union type to appear in the argument list,
  and be manipulated in the same way we do it in C.

  then, Substitution would be defined as something like

  void Substitution(Variable* var, Ast** target, InternedString id, Ast* value, Environment* env);
  void Substitution(Bind* bind, Ast** target, InternedString id, Ast* value, Environment* env);
  ...


  */
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
