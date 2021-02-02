#ifndef VARIABLE_H
#define VARIABLE_H

#include "StringInterner.h"

struct Ast;

typedef struct Variable
{
  InternedString id;
} Variable;

Variable* CreateVariable(InternedString id);

void DestroyVariable(Variable* var);

Variable* CloneVariable(Variable* var);

char* ToStringVariable(Variable* var);

#endif // !VARIABLE_H
