#ifndef INTEGER_H
#define INTEGER_H

#include "TypeJudgement.h"

struct Ast;
struct Environment;

typedef struct Integer
{
  int value;
} Integer;

void InitializeInteger(Integer* integer, int value);

void DestroyInteger(Integer* integer);

void CloneInteger(Integer* destination, Integer* source);

char* ToStringInteger(Integer* integer);

TypeJudgement GetypeInteger(struct Ast* node, struct Environment* env);

#endif // !INTEGER_H
