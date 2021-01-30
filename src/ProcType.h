#ifndef PROCTYPE_H
#define PROCTYPE_H

struct Type;

typedef struct ProcType
{
  struct Type* lhs;
  struct Type* rhs;
} ProcType;

ProcType* CreateProcType(struct Type* left, struct Type* right);

void DestroyProcType(ProcType* proc);

ProcType* CloneProcType(ProcType* proc);

char* ToStringProcType(ProcType* proc);

bool EqualProcTypes(ProcType* t1, ProcType* t2);
#endif // !PROCTYPE_H
