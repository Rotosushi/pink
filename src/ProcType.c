#include <stdlib.h>
#include <string.h>

#include "Type.h"
#include "ProcType.h"

void InitializeProcType(ProcType* proc, struct Type* left, struct Type* right)
{
  proc->lhs = left;
  proc->rhs = right;
}

void DestroyProcType(ProcType* proc)
{
  DestroyType(proc->lhs);
  DestroyType(proc->rhs);
}

void CloneProcType(ProcType* destination, ProcType* source)
{
  CloneType(destination->lhs, source->lhs);
  CloneType(destination->rhs, source->rhs);
}

char* ToStringProcType(ProcType* proc)
{
  char *result, *rarrow, *left, *right;
  rarrow = " -> "; // 4
  left   = ToStringType(proc->lhs);
  right  = ToStringType(proc->rhs);

  int sz = strlen(left) + strlen(right) + 5;
  result = (char*)calloc(sz, sizeof(char));

  strcat(result, left);
  strcat(result, rarrow);
  strcat(result, right);
  return result;
}

bool EqualProcTypes(ProcType* t1, ProcType* t2)
{
  bool result = false;
  Type *t1ltype = t1->lhs, *t1rtype = t1->rhs, *t2ltype = t2->lhs, *t2rtype = t2->rhs;
  result = EqualTypes(t1ltype, t2ltype) || EqualTypes(t1rtype, t2rtype);
  return result;
}
