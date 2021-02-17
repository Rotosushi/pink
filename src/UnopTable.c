#include <stdlib.h>

#include "StringInterner.h"
#include "Type.h"
#include "Ast.h"
#include "UnopEliminators.h"
#include "UnopTable.h"

#define UOPTABLE_DEFAULT_NUM_BUCKETS 10

UnopTable* CreateUnopTable()
{
  UnopTable* result = (UnopTable*)malloc(sizeof(UnopTable));
  result->table     = (UTElem**)malloc(sizeof(UTElem) * UOPTABLE_DEFAULT_NUM_BUCKETS);
  result->num_buckets = UOPTABLE_DEFAULT_NUM_BUCKETS;
  result->num_elements = 0;
  for (int i = 0; i < result->num_buckets; i++)
  {
    result->table[i] = NULL;
  }
  return result;
}

void DestroyUnopTable(UnopTable* ut)
{
  for (int i = 0; i < ut->num_buckets; i++)
  {
    UTElem* head = ut->table[i], *prv = NULL;

    while (head != NULL)
    {
      prv  = head;
      head = head->next;
      DestroyUnopEliminatorList(prv->data);
      free(prv);
    }
  }
  free(ut->table);
  ut->table = NULL;
  ut->num_buckets = 0;
  ut->num_elements = 0;
  free(ut);
  ut = NULL;
}

UnopEliminatorList* InsertUnop(UnopTable* ut, InternedString op, UnopEliminatorList* UElist)
{
  int hash = (int)op;
  hash %= ut->num_buckets;

  UTElem* head = ut->table[hash], *cur = (UTElem*)malloc(sizeof(UTElem));
  cur->op   = op;
  cur->data = UElist;

  cur->next = head;
  ut->table[hash] = cur;
  ut->num_elements++;

  return cur->data;
}

UnopEliminatorList* FindUnop(UnopTable* ut, InternedString op)
{
  int hash = (int)op;
  hash %= ut->num_buckets;

  UTElem* cur = ut->table[hash];

  while (cur != NULL)
  {
    if (cur->op == op)
      return cur->data;

    cur = cur->next;
  }

  return NULL;
}
