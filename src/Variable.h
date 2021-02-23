#ifndef VARIABLE_H
#define VARIABLE_H

#include "TypeJudgement.h"
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

TypeJudgement GetypeVariable(Variable* var, struct Environment* env);

#endif // !VARIABLE_H
