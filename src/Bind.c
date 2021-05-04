#include <stdlib.h>
#include <string.h>

#include "Ast.h"
#include "SymbolTable.h"
#include "Environment.h"
#include "StringInterner.hpp"
#include "Utilities.hpp"
#include "Bind.h"

void InitializeBind(Bind* bind, InternedString id, struct Ast* right, Location* loc)
{
  bind->loc = *loc;
  bind->id  = id;
  bind->rhs = right;
}

void DestroyBind(Bind* bind)
{
  DestroyAst(bind->rhs);
}

void CloneBind(Bind* destination, Bind* source)
{
  destination->loc = source->loc;
  destination->id  = source->id;
  CloneAst(&(destination->rhs), source->rhs);
}

char* ToStringBind(Bind* bnd)
{
  char *result, *clneq, *right, *ctx;
  clneq = " := "; // 4 + 1 = 5
  right = ToStringAst(bnd->rhs);

  // string interning is so, so, so clever. :)
  int sz = strlen((char*)bnd->id) + strlen(right) + 5;
  result = (char*)calloc(sz, sizeof(char));

  strkat(result, (char*)bnd->id, &ctx);
  strkat(result, clneq,          &ctx);
  strkat(result, right,          &ctx);
  free(right);
  return result;
}


Judgement GetypeBind(Bind* bnd, Environment* env)
{
  Judgement result;

  Judgement bound = LookupLocalSymbol(env->symbols, bnd->id);

  if (bound.success == false)
  {
    Judgement rhsjdgmt = Getype(bnd->rhs, env);
    // when we add a sequence (';') grapheme, we need to
    // bind to the type of the rhs (a'la how we type a procedure
    // body already) to properly
    // typecheck terms in our grammar. (otherwise the information
    // is not communicated further down the tree when
    // the next term in the sequence uses data from this
    // binding)
    // because we handle the sequence via recursion.
    // sequence has essentially the same storage pattern as
    // application, assignment, bind, and iteration.
    // all ast members have either one pointer,
    // two pointers, three pointers, or oh goodness four pointers!
    result = rhsjdgmt;
  }
  else
  {
    result.success   = false;
    result.error.dsc = dupstr("Id is already bound in local scope");
    result.error.loc = bnd->loc;
  }
  return result;
}

Judgement EvaluateBind(Bind* bnd, struct Environment* env)
{
  Judgement result;

  Judgement bound = LookupLocalSymbol(env->symbols, bnd->id);

  if (bound.success == false)
  {
    result = Evaluate(bnd->rhs, env);

    if (result.success == true)
    {
      BindSymbol (env->symbols, bnd->id, env->scope, result.term);
    }
  }
  else
  {
    char* es1 = "Id [";
    char* es2 = "] is already bound in scope [";
    char* es3 = "]";
    char* boundtxt   = ToStringAst(bound.term);
    char* I0         = appendstr(es1, bnd->id);
    char* I1         = appendstr(boundtxt, es3);
    char* I2         = appendstr(es2, I1);
    result.success   = false;
    //result.error.dsc = (es1 + bnd->id) + (es2 + (ToStringAst(bound_term) + es3));
    //                        ^          ^      ^  ^^^^^^^^^^^^^^^^^^^^^^  ^
    //                        d          d*     d    dynamic-allocation    d
    // d: dynamic-allocation.
    // d* the allocation that is assigned
    // the dynamic memory associated with anonymous
    // variables can be freed.
    // the dynamic memory that is assigned to variables
    // which do not survive the scope can be freed.
    // the dynamic memory that is returned out of the
    // scope is the callers responsibility to free,
    result.error.dsc = appendstr(I0, I2);
    result.error.loc = bnd->loc;
    free(boundtxt); // each intermediate term is dynamically
    free(I0);       // allocated and as such needs to be freed
    free(I1);       // after use, since the final appendstr allocation is assigned
    free(I2);       // to a term which is returned, it cannot be freed
  }
  return result;
}

bool AppearsFreeBind(Bind* bnd, InternedString id)
{
  // if the name we are binding matches the
  // id we are looking for, the programmer is
  // declaring a new binding in the local scope
  // which shadows the outer binding within the
  // local scope. hence, the name appears bound
  // within the scope, and not free. i suppose that
  // technically we could distinguish between two
  // variables in exactly the rhs of the bind term
  // which introduces the shadow binding, but this
  // introduces a different subtlety that we need
  // to check for. and this is the simpler choice.
  if (bnd->id == id)
    return false;
  else
    return AppearsFree(bnd->rhs, id);
}

void RenameBindingBind(Bind* bnd, InternedString target, InternedString replacement)
{
  if (bnd->id == target)
    return;
  else
    RenameBinding(bnd->rhs, target, replacement);
}

void SubstituteBind(Bind* bnd, Ast** target, InternedString id, Ast* value, struct Environment* env)
{
  if (bnd->id == id)
    return;
  else
    Substitute(bnd->rhs, &(bnd->rhs), id, value, env);
}
