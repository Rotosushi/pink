#include <stdlib.h>
#include <string.h>

#include "Ast.h"
#include "Type.h"
#include "Utilities.h"
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
  CloneType(&(destination->lhs), source->lhs);
  CloneType(&(destination->rhs), source->rhs);
}

bool is_proc_type(Type* type)
{
  bool result = false;
  if (type->kind == T_PROC)
    result = true;
  return result;
}

char* ToStringProcType(ProcType* proc)
{
  char *result, *rarrow, *left, *right, *ctx;
  char *lprn, *rprn;
  rarrow = " -> "; // 4
  lprn   = "(";    // 1
  rprn   = ")";    // 1
  left   = ToStringType(proc->lhs);
  right  = ToStringType(proc->rhs);

  if (is_proc_type(proc->lhs))
  {
    // surround the lhs with parenthesis
    // to denote the function type.
    int sz = strlen(left) + strlen(right) + 7;
    result = (char*)calloc(sz, sizeof(char));

    strkat(result, lprn,   &ctx);
    strkat(NULL,   left,   &ctx);
    strkat(NULL,   rprn,   &ctx);
    strkat(NULL,   rarrow, &ctx);
    strkat(NULL,   right,  &ctx);
  }
  else
  {
    int sz = strlen(left) + strlen(right) + 5;
    result = (char*)calloc(sz, sizeof(char));

    strkat(result, left,   &ctx);
    strkat(NULL,   rarrow, &ctx);
    strkat(NULL,   right,  &ctx);
  }

  free(left);
  free(right);
  return result;
}



bool EqualProcTypes(ProcType* t1, ProcType* t2)
{
  bool result = false;
  Type *t1lhs = t1->lhs, *t1rhs = t1->rhs;
  Type *t2lhs = t2->lhs, *t2rhs = t2->rhs;

  if (t1lhs && t1rhs && t2lhs && t2rhs)
    result = EqualTypes(t1lhs, t2lhs) && EqualTypes(t1rhs, t2rhs);
  return result;
}
