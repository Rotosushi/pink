#ifndef LAMBDA_H
#define LAMBDA_H

#include <stdbool.h>

#include "Judgement.h"
#include "ScopeSet.h"
#include "StringInterner.h"
#include "Location.h"
struct Ast;
struct SymbolTable;
struct Environment;


/*
so, in order to properly translate this language
into assembly we need to handle a few things different
than we can in the interpreter. with the tree style
evaluation we can insert nodes directly into the tree
and thus we can strip one lambda away at a time, and
provide multiple argument procedures without any extra effort.
of course to translate to a c-style procedure which is
applied all at once we need to store intermediate results
from partial application. the straightforward solution is
to store a tuple made up of the partial arguments in a
pair with a pointer to the procedure to apply.

so this presents us with a problem, where do we store the knowledge
that the immediate next term within the body is another lambda?
and how do we handle the fact that we can't know until we parse
the next term, which is within another procedure call. (recursively)
the only way i can think is a dynamic pointer to an int maybe?
what about a boolean stored within the Lambda, which if true, then
the body is another Lambda immediately, i.e. this is a Lambda that
simply participates in argument handling. how does it change when
we observe that we are currying? store something in env?
when is it safe to change the state?

PLam -> PAffix -> PLam -> PAffix -> PLam -> PAffix -> anything-else

  I can actually solve this in the 'constructor'
  for lambda Ast terms. if the immediate node in
  the body tree of a lambda is itself another lambda,
  we are observing a curryed lambda. otherwise, if
  the parser returns an application or a binop term
  we know that applying this procedure results in
  jumping into other operations. and any procedure
  definitions occurring within the body term, but
  not as the immediate node from the body ptr,
  are considered locally scoped definitions.

*/
typedef struct Lambda
{
  Location            loc;
  InternedString      arg_id;
  struct Ast*         arg_type;
  struct Ast*         body;
  struct SymbolTable* symbols;
  ScopeSet            scope;
  bool                curryed;
  int                 arg_num;
} Lambda;

void InitializeLambda(Lambda* lambda, InternedString arg_id, struct Ast* arg_type, struct Ast* body, struct SymbolTable* symbols, ScopeSet scope, Location* loc);

void DestroyLambda(Lambda* lam);

void CloneLambda(Lambda* destination, Lambda* source);

char* ToStringLambda(Lambda* lam);

Judgement GetypeLambda(Lambda* node, struct Environment* env);

bool AppearsFreeLambda(Lambda* lam, InternedString id);

void RenameBindingLambda(Lambda* lam, InternedString target, InternedString replacement);

void SubstituteLambda(Lambda* lam, struct Ast** target, InternedString id, struct Ast* value, struct Environment* env);

#endif // !LAMBDA_H
