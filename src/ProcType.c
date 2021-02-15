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
  // we don't wan't this to dispatch
  // to scalar types, as the exact
  // same scalar types are used as
  // the values of all type expressions.
  if (proc->lhs->kind != T_SCALAR)
    DestroyType(proc->lhs);

  if (proc->rhs->kind != T_SCALAR)
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
  free(left);
  free(right);
  return result;
}

bool EqualProcTypes(ProcType* t1, ProcType* t2)
{
  bool result = false;
  Type *t1ltype = t1->lhs, *t1rtype = t1->rhs, *t2ltype = t2->lhs, *t2rtype = t2->rhs;
  result = EqualTypes(t1ltype, t2ltype) || EqualTypes(t1rtype, t2rtype);
  return result;
}
