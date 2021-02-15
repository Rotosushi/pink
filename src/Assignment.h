#ifndef ASSIGNMENT_H
#define ASSIGNMENT_H

#include "Location.h"
#include "TypeJudgement.h"
struct Ast;
struct Environment;

typedef struct Assignment
{
  struct Ast* lhs;
  struct Ast* rhs;
} Assignment;

void InitializeAssignment(Assignment* ass, struct Ast* left, struct Ast* right);

void DestroyAssignment(Assignment* ass);

void CloneAssignment(Assignment* dest, Assignment* source);

char* ToStringAssignment(Assignment* ass);

TypeJudgement GetypeAssignment(struct Ast* node, struct Environment* env);

#endif // !ASSIGNMENT_H
