#include <stdlib.h>
#include <string.h>

#include "Ast.h"
#include "Location.h"
#include "Utilities.h"
#include "Type.h"
#include "StringInterner.h"
#include "Environment.h"
#include "SymbolTable.h"
#include "Lambda.h"

bool is_lambda_term(Ast* term)
{
  bool result = false;
  if (term->kind == A_OBJ && term->obj.kind == O_LAMB)
    result = true;
  return result;
}

void InitializeLambda(Lambda* lambda, InternedString arg_id, struct Ast* arg_type, struct Ast* body, struct SymbolTable* symbols, Location* loc)
{
  lambda->loc        = *loc;
  lambda->arg_id     = arg_id;
  lambda->arg_type   = arg_type;
  lambda->body       = body;
  lambda->symbols    = symbols;
  if (is_lambda_term(body))
    lambda->curryed = true;
  else
    lambda->curryed = false;
}

void DestroyLambda(Lambda* lam)
{
  DestroyAst(lam->body);
  DestroySymbolTable(lam->symbols);
}

void CloneLambda(Lambda* destination, Lambda* source)
{
  destination->loc      = source->loc;
  destination->arg_id   = source->arg_id;

  CloneAst(&(destination->arg_type), source->arg_type);
  CloneAst(&(destination->body), source->body);
  CloneSymbolTable(&(destination->symbols), source->symbols);
}

char* ToStringLambda(Lambda* lam)
{
  char *result = NULL, *argtype = NULL, *body = NULL, *ctx = NULL;
  char *bslsh = NULL, *cln = NULL, *eqrarrow = NULL;
  bslsh    = "\\"; // 1
  cln      = ": "; // 2
  eqrarrow = " => "; // 4
  argtype  = ToStringAst(lam->arg_type);
  body     = ToStringAst(lam->body);


  // 7 extra characters and a null terminator walk into a string...
  int sz = strlen(argtype) + strlen(body) + 8;
  result = (char*)calloc(sz, sizeof(char));

  strkat(result, bslsh,             &ctx);
  strkat(NULL,  (char*)lam->arg_id, &ctx);
  strkat(NULL,  cln,                &ctx);
  strkat(NULL,  argtype,            &ctx);
  strkat(NULL,  eqrarrow,           &ctx);
  strkat(NULL,  body,               &ctx);
  free(argtype);
  free(body);
  return result;
}
// given that the argument has type Ta
// and we can type the body as having
// type Tb, then we can conclude that
// the procedure has type Ta -> Tb
Judgement GetypeLambda(Lambda* lam, Environment* env)
{
  Judgement result;
  Ast* dummy_binding = NULL;

  CloneAst(&(dummy_binding), lam->arg_type);

  ScopeSet     lastscope   = env->scope;
  SymbolTable* lastsymbols = env->symbols;
                                        // when we enter the body
                                        // of the lambda we also
                                        // enter it's local scope.
  env->symbols = lam->symbols;          // so we save/restore the
  env->scope   = lam->scope;            // active scope, as well
                                        // as the bindings within
                                        // the current scope.
                                        // and since this is interpretation
                                        // everything is heap allocated
                                        // we can do easy pointer assignment.

  BindSymbol(env->symbols, lam->arg_id, localscope, dummy_binding);
                                               // the symbol table currently
                                               // takes ownership of the Ast
                                               // tree's that it binds.
                                               // which is why we need to bind to
                                               // a clone of the type term.

  Judgement bodyjdgmt = Getype(lam->body, env); // Getype returns terms which
                                                // represent type

  UnbindSymbol(env->symbols, lam->arg_id); // this line deallocates the
                                   // memory associated with the interned string
                                   // ((not the 'type' though, we still
                                   //   intern types remember?))

  env->symbols = lastsymbols; // restore the previous scope and symbols
  env->scope   = lastscope;

  if (bodyjdgmt.success == true)
  {
    result.success = true;
    result.term    = GetProcedureType(env->interned_types, lam->arg_type, bodyjdgmt.term, &(lam->loc));
  }
  else
    result = bodyjdgmt;

  return result;
}


bool AppearsFreeLambda(Lambda* lam, InternedString id)
{
  // if the arg matches the id we are looking for,
  // the name appears bound in the lower scope
  // not free.
  if (lam->arg_id == id)
    return false;
  else
    return AppearsFree(lam->body, id);
}

void RenameBindingLambda(Lambda* lam, InternedString target, InternedString replacement)
{
  if (lam->arg_id == target)
    return;
  else
    RenameBinding(lam->body, target, replacement);
}

void SubstituteLambda(Lambda* lam, Ast** target, InternedString id, Ast* value, struct Environment* env)
{
  // if this lambda term introduces a binding that
  // matches within it's own scope, to substitute for
  // that binding within that term is a mistake.
  if (lam->arg_id == id)
    return;

  // this is now the old solution,
  // now, when lookup occurs we use
  // scopesets to disambiguate between
  // references being substituted in.
  // the binding being introduced by this
  // lambda will have an additional scope
  // associated with it, thus when the value
  // is substituted in, the free variable will
  // not be associated with the variable within
  // the scope that the lambda introduces.
  //  this means that the
  // outer binding will be a better match for the
  // free variable within the value being substituted.
  // (can it be that the lambda captures
  // a free variable introduced within a subscope, and
  // thus the lookup would fail, were we to not close over
  // those values at the time when we return the lambda?
  // i beleive so yes.)
  // if the binding that this lambda introduces appears
  // free in the value we are substituting in, then
  // we would introduce an unintentional binding by
  // our substitution operation, and so we need to rename
  // the conflicting binding within the lambda we are substituting
  // into.
  /*
  else if (AppearsFree(value, lam->arg_id))
  {
    char* new_id = gensym(NULL);
    InternedString old_name = lam->arg_id;
    InternedString new_name = InternString(env->interned_ids, new_id);

    // if we are calling substitute on a lambda term,
    // we must be operating on a copy of the lambda
    // term actually bound. to ensure we don't modify
    // the bound term. because we must ensure we don't
    // inadvertently bind the free name in the value
    // to the bound name present in this lambda term.
    lam->arg_id = new_name;
    RenameBinding(lam->body, old_name, new_name);

  }
  */

  Substitute(lam->body, &(lam->body), id, value, env);
}
