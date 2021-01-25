#ifndef VARIABLE_H
#define VARIABLE_H

#include "StringInterner.h"

struct Ast;

typedef struct Variable
{
  InternedString id;
} Variable;

void DestroyVariable(Variable* var);

Variable* CloneVariable(Variable* var);

char* TostringVariable(Variable* var);

#endif // !VARIABLE_H
