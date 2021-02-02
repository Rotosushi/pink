// implements a map between
// interned binops and pointers
// to that binops eliminator set.
#ifndef BINOPTABLE_H
#define BINOPTABLE_H
#include <stdlib.h>

#include "StringInterner.h"
#include "BinopEliminators.h"

typedef struct BTElem
{
  InternedString       op;
  BinopEliminatorList *data;
  struct BTElem       *next;
} BTElem;

typedef struct BinopTable
{
  BTElem** table;
  int num_buckets;
  int num_elements;
} BinopTable;

BinopTable* CreateBinopTable();
void        DestroyBinopTable(BinopTable* BOPTable);
BinopEliminatorList* InsertBinop(BinopTable* BOPTable, InternedString op, BinopEliminatorList* BEList);
BinopEliminatorList* FindBinop(BinopTable* BOPTable, InternedString op);
#endif // !UNOPTABLE_H
