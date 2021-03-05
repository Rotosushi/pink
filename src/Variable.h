#ifndef VARIABLE_H
#define VARIABLE_H

#include "Judgement.h"
#include "Judgement.h"
#include "StringInterner.h"
#include "Location.h"
struct Environment;
struct Ast;

typedef struct Variable
{
  Location       loc;
  InternedString id;
} Variable;

void InitializeVariable(Variable* var, InternedString id, Location* loc);

void DestroyVariable(Variable* var);

void CloneVariable(Variable* destination, Variable* source);

char* ToStringVariable(Variable* var);

Judgement GetypeVariable(Variable* var, struct Environment* env);

Judgement EvaluateVariable(Variable* var, struct Environment* env);

#endif // !VARIABLE_H
