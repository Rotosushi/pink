#ifndef VARIABLE_H
#define VARIABLE_H

#include "StringInterner.h"

struct Environment;
struct Ast;

typedef struct Variable
{
  InternedString id;
} Variable;

variable InitializeVariable(Variable* var, InternedString id);

void DestroyVariable(Variable* var);

void CloneVariable(Variable* destination, Variable* source);

char* ToStringVariable(Variable* var);

TypeJudgement GetypeVariable(Variable* var, struct Environment* env);

#endif // !VARIABLE_H
