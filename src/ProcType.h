#ifndef PROCTYPE_H
#define PROCTYPE_H

struct Ast;

typedef struct ProcType
{
  struct Ast* lhs;
  struct Ast* rhs;
} ProcType;

void InitializeProcType(ProcType* proc, struct Ast* left, struct Ast* right);

void DestroyProcType(ProcType* proc);

void CloneProcType(ProcType* destination, ProcType* source);

char* ToStringProcType(ProcType* proc);

bool EqualProcTypes(ProcType* t1, ProcType* t2);
#endif // !PROCTYPE_H
