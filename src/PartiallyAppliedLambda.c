#include <stdlib.h>
#include <string.h>


#include "Ast.h"
#include "Location.h"
#include "Utilities.hpp"
#include "Type.h"
#include "StringInterner.hpp"
#include "Environment.h"
#include "SymbolTable.h"


void InitializePartiallyAppliedLambda(PartiallyAppliedLambda* parapp, struct Ast* rest, InternedString arg_name, ScopeSet arg_scope, struct Ast* arg_value, struct Ast* arg_type, Location* loc)
{
  parapp->loc           = *loc;
  parapp->argslen       = 1;
  parapp->args          = (PartialArgument*)malloc(sizeof(PartialArgument));
  parapp->rest          = rest;
  parapp->args[0].name  = arg_name;
  parapp->args[0].scope = arg_scope;
  parapp->args[0].value = arg_value;
  parapp->args[0].type  = arg_type;
}

void DestroyPartiallyAppliedLambda(PartiallyAppliedLambda* parapp)
{
  for (int i = 0; i < parapp->argslen; i++)
  {
    DestroyAst(parapp->args[i].value);
    DestroyAst(parapp->args[i].type);
  }
  free(parapp->args);
  // so this is destroying a Lambda object,
  // however it is currently garunteed to be
  // a copy, so further thinking needs done.
  DestroyAst(parapp->rest);
}

void ClonePartiallyAppliedLambda(PartiallyAppliedLambda* dest, PartiallyAppliedLambda* source)
{
  for (int i = 0; i < dest->argslen; i++)
  {
    DestroyAst(dest->args[i].value);
    DestroyAst(dest->args[i].type);
  }
  free(dest->args);
  if (dest->rest != NULL)
    DestroyAst(dest->rest);

  dest->args = (PartialArgument*)malloc(sizeof(PartialArgument) * source->argslen);

  for (int i = 0; i < source->argslen; i++)
  {
    dest->args[i].name = source->args[i].name;
    CloneAst(&(dest->args[i].value), source->args[i].value);
    CloneAst(&(dest->args[i].type),  source->args[i].type);
  }

  CloneAst(&(dest->rest), source->rest);
}

char* ToStringPartiallyAppliedLambda(PartiallyAppliedLambda* parapp)
{
  char* lbrace = "[", *rbrace = "]", *comma = ",", *colonspc = ": ";
  char* rest = ToStringAst(parapp->rest);
  char* result = NULL, *partialargs = lbrace;

  if (parapp->argslen == 1)
  {
    char* value  = ToStringAst(parapp->args[0].value);
    char* t0     = appendstr(partialargs, parapp->args[0].name->c_str());
    char* t1     = appendstr(t0, colonspc);
    char* t2     = appendstr(t1, value);
    partialargs  = appendstr(t2, rbrace);
    free(value);
    free(t0);
    free(t1);
    free(t2);
  }
  else
  {
    for (int i = 0; i < parapp->argslen; i++)
    {
      char* type = ToStringAst(parapp->args[i].type);
      char* t0   = appendstr(partialargs, parapp->args[i].name->c_str());
      char* t1   = appendstr(t0, colonspc);
      char* t2   = appendstr(t1, type);

      if (i < (parapp->argslen - 1))
      {
        partialargs = appendstr(t2, comma);
        free(t2);
      }
      else
      {
        partialargs = t2;
      }
      free(type);
      free(t0);
      free(t1);
    }

    char* tmp = partialargs;
    partialargs = appendstr(partialargs, rbrace);
    free(tmp);
  }
  result = appendstr(partialargs, rest);
  free(partialargs);
  free(rest);
  return result;
}

// the type of the partial application is the
// type of the current argument '->'
// type of rest with every argument bound to
// it's dummy type within the scope before typing.
// (typing the lambda itself will already bind each
// argument for typing.)
Judgement GetypePartiallyAppliedLambda(PartiallyAppliedLambda* parapp, struct Environment* env)
{
  // normally smelly code, but PartiallyAppliedLambda literally
  // only deals in Lambda terms, because it is -only- constructed
  // by EvaluateApplication, and only after we peel a lambda off
  // and find another lambda ourselves, and EvaluateApplication is
  // written to handle a PartiallyAppliedLambda in a lhs position.
  SymbolTable* partial_symbols = parapp->rest->obj.lambda.symbols;
  Judgement result;
  Location  loc = parapp->rest->obj.lambda.loc;
  Ast* lhstype;
  CloneAst(&(lhstype), parapp->rest->obj.lambda.arg_type);

  for (int i = 0; i < parapp->argslen; i++)
  {
    Ast* dummy_binding = NULL;
    CloneAst(&dummy_binding, parapp->args[i].type);
    // the symboltable takes ownership of the bound term,
    // so we need to clone here to ensure we don't segfault,
    // after unbinding the symbols.
    BindSymbol(partial_symbols, parapp->args[i].name, parapp->args[i].scope, dummy_binding);
  }

  // we have bound the held symbols to their type and thus we
  // need to typecheck the 'body' of the PAL against the provided
  // argument types.
  Judgement rest = Getype(parapp->rest, env);

  for (int i = 0; i < parapp->argslen; i++)
  {
    UnbindSymbol(partial_symbols, parapp->args[i].name);
  }


  if (rest.success == true)
  {
    result.success = true;
    result.term    = CreateAstType(
                      GetProcedureType(
                        env->interned_types,
                        GetTypePtrFromLiteral(lhstype),
                        GetTypePtrFromLiteral(rest.term)),
                      &loc);
  }
  else
  {
    result = rest;
  }
  return result;
}

// so the common action is:
// here, i just evaluated the next argument
// down to a value, so you need to peel off
// the curryied lambda term, down to the next
// lambda term, and add the new argument to your
// argument list.
// however, if we find ourselves within the
// situation where the rest of the application
// isn't curryied then we are free to perform
// every substitution we have built up.
// the name, scope, and type can be retreived from
// the lambda we are peeling away.
Judgement ApplyArgument(Ast* ast,  \
                        Ast* arg_value, \
                        Environment* env)
{
  Judgement result;
  PartiallyAppliedLambda* parapp = &(ast->obj.parapp);
  Lambda*        lambda      = &(parapp->rest->obj.lambda);
  InternedString arg_name    = lambda->arg_id;
  ScopeSet       arg_scope   = lambda->scope;
  Ast*           arg_type    = NULL;
  CloneAst(&arg_type, lambda->arg_type);

  if (lambda->curryed == true)
  {
    parapp->argslen += 1;
    parapp->args     = (PartialArgument*)realloc(parapp->args, sizeof(PartialArgument) * parapp->argslen);
    parapp->args[parapp->argslen - 1].name  = arg_name;
    parapp->args[parapp->argslen - 1].scope = arg_scope;
    parapp->args[parapp->argslen - 1].value = arg_value;
    parapp->args[parapp->argslen - 1].type  = arg_type;

    parapp->rest   = lambda->body;
    result.success = true;
    result.term    = ast;
  }
  else
  {
    // if this lambda is not curryed, then when we
    // add this argument we are now in a position to
    // apply this procedure fully, because the lambda->body
    // pointer will no longer refer to another Lambda object,
    // but the body of the rightmost lambda object in the
    // curry chain, i.e. the end of the formal argument list.
    // essentially, here, we can sidestep adding the new argument
    // to the list, and instead perform the usual application.
    // i suppose there is an argument for a substitute procedure
    // which takes the list of arguments and performs all of the
    // substitutions along a single walk of the tree.
    for (int i = 0; i < parapp->argslen; i++)
    {
      Substitute(lambda->body, &(lambda->body), parapp->args[i].name, parapp->args[i].value, env);
    }

    Substitute(lambda->body, &(lambda->body), arg_name, arg_value, env);

    SymbolTable* lastsymbols = env->symbols;
    ScopeSet     lastscope   = env->scope;

    env->symbols = lambda->symbols;
    env->scope   = lambda->scope;

    result = Evaluate(lambda->body, env);

    env->symbols = lastsymbols;
    env->scope   = lastscope;
  }
  return result;
}
