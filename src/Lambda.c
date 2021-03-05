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

void InitializeLambda(Lambda* lambda, InternedString arg_id, struct Ast* arg_type, struct Ast* body, struct SymbolTable* scope, Location* loc)
{
  lambda->loc      = *loc;
  lambda->arg_id   = arg_id;
  lambda->arg_type = arg_type;
  lambda->body     = body;
  lambda->scope    = scope;
  if (is_lambda_term(body))
    lambda->curryed = true;
  else
    lambda->curryed = false;
}

void DestroyLambda(Lambda* lam)
{
  DestroyAst(lam->body);
  DestroySymbolTable(lam->scope);
}

void CloneLambda(Lambda* destination, Lambda* source)
{
  destination->loc      = source->loc;
  destination->arg_id   = source->arg_id;

  CloneAst(&(destination->arg_type), source->arg_type);
  CloneAst(&(destination->body), source->body);
  CloneSymbolTable(&(destination->scope), source->scope);
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

  SymbolTable* last_scope = env->outer_scope; // when we enter the body
                                        // of the lambda we also
                                        // enter it's local scope.
  env->outer_scope = lam->scope;              // so we save/restore the
                                        // active scope, as well
                                        // as the bindings within
                                        // the current scope.
                                        // and since this is interpretation
                                        // everything is heap allocated
                                        // we can do easy pointer assignment.

  bind(env->outer_scope, lam->arg_id, dummy_binding); // the symbol table currently
                                               // takes ownership of the Ast
                                               // tree's that it binds.
                                               // which is why we need to bind to
                                               // a clone of the type term.

  Judgement bodyjdgmt = Getype(lam->body, env); // Getype returns terms which
                                                // represent type

  unbind(env->outer_scope, lam->arg_id); // this line deallocates the
                                   // memory associated with the interned string
                                   // ((not the 'type' though, we still
                                   //   intern types remember?))

  env->outer_scope = last_scope; // restore the previous scope.

  if (bodyjdgmt.success == true)
  {
    result.success = true;
    result.term    = CreateAstType(GetProcedureType(env->interned_types, lam->arg_type, bodyjdgmt.term), &(lam->loc));
  }
  else
    result = bodyjdgmt;

  return result;
}
