#include <stdlib.h>
#include <string.h>

#include "Ast.h"
#include "Type.h"
#include "StringInterner.h"
#include "Environment.h"
#include "SymbolTable.h"
#include "Lambda.h"

void InitializeLambda(Lambda* lambda, InternedString arg_id, struct Type* arg_type, struct Ast* body, struct SymbolTable* scope)
{
  lambda->arg_id   = arg_id;
  lambda->arg_type = arg_type;
  lambda->body     = body;
  lambda->scope    = scope;
}

void DestroyLambda(Lambda* lam)
{
  DestroyAst(lam->body);
  DestroySymbolTable(lam->scope);
}

void CloneLambda(Lambda* destination, Lambda* source)
{
  destination->arg_id   = source->arg_id;
  destination->arg_type = source->arg_type;

  CloneAst(&(destination->body), source->body);

  CloneSymbolTable(&(destination->scope), source->scope);
}

char* ToStringLambda(Lambda* lam)
{
  char *result, *argtype, *body;
  char *bslsh, *cln, *eqrarrow;
  bslsh    = "\\"; // 1
  cln      = ": "; // 2
  eqrarrow = " => "; // 4
  argtype  = ToStringType(lam->arg_type);
  body     = ToStringAst(lam->body);

  // 7 extra characters and a null terminator walk into a string...
  int sz = strlen(argtype) + strlen(body) + 8;
  result = (char*)calloc(sz, sizeof(char));

  strcat(result, bslsh);
  strcat(result, (char*)lam->arg_id);
  strcat(result, cln);
  strcat(result, argtype);
  strcat(result, eqrarrow);
  strcat(result, body);
  free(argtype);
  free(body);
  return result;
}

// given that the argument has type Ta
// and we can type the body as having
// type Tb, then we can conclude that
// the procedure has type Ta -> Tb
TypeJudgement GetypeLambda(Ast* node, Environment* env)
{
  TypeJudgement result;
  Lambda* lam = &(node->obj.lambda);

  // in order to get the body to typecheck
  // we must provide some way to type the
  // binding that is supposed to be created
  // by calling a procedure. this is accomplished
  // by binding the name to a type literal just
  // as long as it takes to typecheck the body
  // tree.
  Ast* type_literal = CreateType(lam->arg_type, &(node->loc));

  SymbolTable* last_scope = env->outer_scope; // when we enter the body
                                        // of the lambda we also
                                        // enter it's local scope.
  env->outer_scope = lam->scope;              // so we save/restore the
                                        // active scope, as well
                                        // as the bindings within
                                        // the current scope.

  bind(env->outer_scope, lam->arg_id, type_literal); // the symbol table currently
                                               // takes ownership of the Ast
                                               // tree's that it binds.

  TypeJudgement bodyjdgmt = Getype(lam->body, env);

  unbind(env->outer_scope, lam->arg_id); // which means this line deallocates the
                                   // memory associated with (type_literal)
                                   // ((not the 'type' though, we still
                                   //   intern types remember?))

  env->outer_scope = last_scope; // restore the previous scope.

  if (bodyjdgmt.success == true)
  {
    result.success = true;
    result.type    = GetProcedureType(env->interned_types, lam->arg_type, bodyjdgmt.type);
  }
  else
    result = bodyjdgmt;

  return result;
}
