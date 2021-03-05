#ifndef ASSIGNMENT_H
#define ASSIGNMENT_H

#include "Location.h"
#include "Judgement.h"
struct Ast;
struct Environment;

typedef struct Assignment
{
  Location    loc;
  struct Ast* lhs;
  struct Ast* rhs;
} Assignment;

void InitializeAssignment(Assignment* ass, struct Ast* left, struct Ast* right, Location* loc);

void DestroyAssignment(Assignment* ass);

void CloneAssignment(Assignment* dest, Assignment* source);

char* ToStringAssignment(Assignment* ass);

Judgement GetypeAssignment(Assignment* ass, struct Environment* env);

Judgement EvaluateAssignment(Assignment* ass, struct Environment* env);

#endif // !ASSIGNMENT_H
