#ifndef PROCTYPE_H
#define PROCTYPE_H

struct Type;

typedef struct ProcType
{
  struct Type* lhs;
  struct Type* rhs;
} ProcType;

void InitializeProcType(ProcType* proc, struct Type* left, struct Type* right);

void DestroyProcType(ProcType* proc);

void CloneProcType(ProcType* destination, ProcType* source);

char* ToStringProcType(ProcType* proc);

bool EqualProcTypes(ProcType* t1, ProcType* t2);
#endif // !PROCTYPE_H
