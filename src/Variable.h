#ifndef VARIABLE_H
#define VARIABLE_H

#include "Judgement.h"
#include "ScopeSet.h"
#include "StringInterner.h"
#include "Location.h"
struct Environment;
struct Ast;

typedef struct Variable
{
  Location       loc;
  ScopeSet       scope;
  InternedString id;
} Variable;

void InitializeVariable(Variable* var, InternedString id, ScopeSet scope, Location* loc);

void DestroyVariable(Variable* var);

void CloneVariable(Variable* destination, Variable* source);

char* ToStringVariable(Variable* var);

Judgement GetypeVariable(Variable* var, struct Environment* env);

Judgement EvaluateVariable(Variable* var, struct Environment* env);

bool AppearsFreeVariable(Variable* var, InternedString id);

void RenameBindingVariable(Variable* var, InternedString target, InternedString replacement);

void SubstituteVariable(Variable* var, struct Ast** target, InternedString id, struct Ast* value, struct Environment* env);

#endif // !VARIABLE_H
