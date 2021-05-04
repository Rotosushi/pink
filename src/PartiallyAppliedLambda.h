#ifndef PARTIAL_APPLICATION_H
#define PARTIAL_APPLICATION_H

#include "Judgement.h"
#include "StringInterner.hpp"
#include "ScopeSet.h"
#include "Location.h"
struct Ast;
struct Environment;

/*
  okay, so we want to support partial application
  in an imperitive language, and my technique is simply
  going to be when an application term is not the final
  term before the start of the body, i.e. the procedure
  is curried, instead of performing the substitution immediately,
  (which works for interpretation, but not compilation), we
  instead start building up a list of (name,value) pairs,
  once we reach the final application, i.e. we are applying
  a non-curryed procedure, we do all of the substitutions from
  the list at once, along with the final substitution.
  thus, a partially applyed C procedure, is simply a pointer
  to the original procedure, plus this list of already evaluated
  arguments. we can accept this partial application object in place of a
  lambda object directly, (because it holds the next lambda object
  directly in interpretation, and is always the same function pointer in
  compilation.) this partial application is valid to be stored as
  a value, with size equal to a function pointer plus the list object.
  the type of the partial application is the remaining type of
  the procedure after the already given arguments have been
  bound to dummy typeliterals for typing.
*/

typedef struct PartialArgument
{
  InternedString   name;
  ScopeSet         scope;
  struct Ast*      value;
  struct Ast*      type;
} PartialArgument;

typedef struct PartiallyAppliedLambda
{
  Location         loc;
  struct Ast*      rest;
  PartialArgument* args;
  int              argslen;
} PartiallyAppliedLambda;


void InitializePartiallyAppliedLambda(PartiallyAppliedLambda* parapp, struct Ast* rest, InternedString arg_name, ScopeSet arg_scope, struct Ast* arg_value, struct Ast* arg_type, Location* loc);

void DestroyPartiallyAppliedLambda(PartiallyAppliedLambda* parapp);

void ClonePartiallyAppliedLambda(PartiallyAppliedLambda* dest, PartiallyAppliedLambda* source);

char* ToStringPartiallyAppliedLambda(PartiallyAppliedLambda* parapp);

Judgement GetypePartiallyAppliedLambda(PartiallyAppliedLambda* parapp, struct Environment* env);

// so the common action is truly,
// here, i just evaluated the next argument
// down to a value, so you need to peel off
// the curryied lambda term, down to the next
// lambda term, and add the new argument to your
// argument list.
// however, if we find ourselves within the
// situation where the rest of the application
// isn't curryied then we are free to perform
// every substitution we have built up and
// evaluate the lambda itself. (this essentially
// allows the programmer to delay evaluation on demand.)
Judgement ApplyArgument(struct Ast* ast, struct Ast* arg_value, struct Environment* env);


#endif // !PARTIAL_APPLICATION_H
