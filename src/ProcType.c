#include <stdlib.h>
#include <string.h>

#include "Type.h"
#include "ProcType.h"

void DestroyProcType(ProcType* proc)
{
  DestroyType(proc->lhs);
  DestroyType(proc->rhs);
  free(proc);
  proc = NULL;
}

ProcType* CloneProcType(ProcType* proc)
{
  ProcType* result = (ProcType*)malloc(sizeof(ProcType));
  result->lhs = CloneType(proc->lhs);
  result->rhs = CloneType(proc->rhs);
  return result;
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
