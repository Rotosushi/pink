#ifndef ASSIGNMENT_H
#define ASSIGNMENT_H

#include "Typejudgement.h"
struct Ast;
struct Environment;

typedef struct Assignment
{
  struct Ast* lhs;
  struct Ast* rhs;
} Assignment;

Assignment* CreateAssignment(struct Ast* left, struct Ast* right);

void DestroyAssignment(Assignment* ass);

Assignment* CloneAssignment(Assignment* ass);

char* ToStringAssignment(Assignment* ass);

TypeJudgement GetypeAssignment(Assignment* ass, Environment* env);

#endif // !ASSIGNMENT_H
