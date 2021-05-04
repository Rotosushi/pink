// Implements a mapping between
// interned unops and their
// elimantor set.
#ifndef UNOPTABLE_H
#define UNOPTABLE_H
#include <stdlib.h>

#include "StringInterner.hpp"
#include "UnopEliminators.h"

typedef struct UTElem
{
  InternedString      op;
  UnopEliminatorList *data;
  struct UTElem      *next;
} UTElem;

typedef struct UnopTable
{
  UTElem** table;
  int num_buckets;
  int num_elements;
} UnopTable;

UnopTable*          CreateUnopTable();
void                DestroyUnopTable(UnopTable* ut);
UnopEliminatorList* InsertUnop(UnopTable* ut, InternedString op, UnopEliminatorList* UElist);
UnopEliminatorList* FindUnop(UnopTable* ut, InternedString op);
#endif // !UNOPTABLE
