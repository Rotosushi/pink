#include <stdlib.h>

#include "StringInterner.h"
#include "Ast.h"
#include "Type.h"
#include "BinopEliminators.h"
#include "BinopTable.h"

#define BOPTABLE_DEFAULT_NUM_BUCKETS 10

BinopTable* CreateBinopTable()
{
  BinopTable* result   = (BinopTable*)malloc(sizeof(BinopTable));
  result->table        = (BTElem**)malloc(sizeof(BTElem*) * BOPTABLE_DEFAULT_NUM_BUCKETS);
  result->num_buckets  = BOPTABLE_DEFAULT_NUM_BUCKETS;
  result->num_elements = 0;
  for (int i = 0; i < result->num_buckets; i++)
    result->table[i] = NULL;
  return result;
}

void DestroyBinopTable(BinopTable* BOPTable)
{
  for (int i = 0; i < BOPTable->num_buckets; i++)
  {
    BTElem* cur = BOPtable->table[i], *prv;
    while(cur != NULL)
    {
      prv = cur;
      cur = cur->next;
      DestroyBinopEliminatorList(prv->data);
      free(prv);
    }
  }
  free(BOPTable->table);
  BOPTable->table = NULL;
  BOPTable->num_buckets = 0;
  BOPTable->num_elements = 0;
  free(BOPTable);
  BOPTable = NULL;
}

BinopEliminatorList* InsertBinop(BinopTable* BOPTable, InternedString op, BinopEliminatorList* BEList)
{
  int hash = (int)op;
  hash %= BOPTable->num_buckets;

  BTElem* head = BOPTable->table[hash], *cur = (BTElem*)malloc(sizeof(BTElem));
  cur->op   = op;
  cur->data = BElist;

  // prepend to list
  cur->next = head;
  head = cur;

  return cur->data;
}

BinopEliminatorList* FindBinop(BinopTable* BOPTable, InternedString op)
{
  int hash = (int)op;
  hash %= BOPTable->num_buckets;

  BTElem* cur = BOPTable->table[hash];

  while(cur != NULL)
  {
    if (cur->op == op)
      return cur->data;

    cur = cur->next;
  }

  return NULL;
}
