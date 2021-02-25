#include <stdlib.h>
#include <string.h>

#include "Ast.h"
#include "Type.h"
#include "Utilities.h"
#include "ProcType.h"

void InitializeProcType(ProcType* proc, struct Ast* left, struct Ast* right)
{
  proc->lhs = left;
  proc->rhs = right;
}

void DestroyProcType(ProcType* proc)
{
  DestroyAst(proc->lhs);
  DestroyAst(proc->rhs);
}

void CloneProcType(ProcType* destination, ProcType* source)
{
  CloneAst(&(destination->lhs), source->lhs);
  CloneAst(&(destination->rhs), source->rhs);
}

bool is_proc_type(Ast* ast)
{
  bool result = false;
  if (ast->kind == A_OBJ)
  {
    if (ast->obj.kind == O_TYPE)
    {
      if (ast->obj.type.literal->kind == T_PROC)
      {
        result = true;
      }
    }
  }
  return result;
}

char* ToStringProcType(ProcType* proc)
{
  char *result, *rarrow, *left, *right, *ctx;
  char *lprn, *rprn;
  rarrow = " -> "; // 4
  lprn   = "(";    // 1
  rprn   = ")";    // 1
  left   = ToStringAst(proc->lhs);
  right  = ToStringAst(proc->rhs);

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

Type* get_type_ptr(Ast* ast)
{
  Type* result = NULL;
  if (ast->kind == A_OBJ)
  {
    if (ast->obj.kind == O_TYPE)
    {
      result = ast->obj.type.literal;
    }
  }
  return result;
}

bool EqualProcTypes(ProcType* t1, ProcType* t2)
{
  bool result = false;
  Type *t1lhs = get_type_ptr(t1->lhs), *t1rhs = get_type_ptr(t1->rhs);
  Type *t2lhs = get_type_ptr(t2->lhs), *t2rhs = get_type_ptr(t2->rhs);

  if (t1lhs && t1rhs && t2lhs && t2rhs)
    result = EqualTypes(t1lhs, t2lhs) && EqualTypes(t1rhs, t2rhs);
  return result;
}
