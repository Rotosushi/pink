#ifndef ASSIGNMENT_H
#define ASSIGNMENT_H

struct Ast;

typedef struct Assignment
{
  struct Ast* lhs;
  struct Ast* rhs;
} Assignment;

Assignment* CreateAssignment(struct Ast* left, struct Ast* right);

void DestroyAssignment(Assignment* ass);

Assignment* CloneAssignment(Assignment* ass);

char* ToStringAssignment(Assignment* ass);

#endif // !ASSIGNMENT_H
