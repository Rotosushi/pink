#include <string.h>
#include <stdlib.h>

#include "StringInterner.h"
#include "Associativity.h"
#include "BinopPrecedence.h"

#define BPATABLE_DEFAULT_NUM_BUCKETS 10

BinopPrecedenceTable* CreateAstBinopPrecedenceTable()
{
  BinopPrecedenceTable* result = (BinopPrecedenceTable*)malloc(sizeof(BinopPrecedenceTable));
  result->table                = (BPElem**)malloc(sizeof(BPElem*) * BPATABLE_DEFAULT_NUM_BUCKETS);
  result->num_buckets          = BPATABLE_DEFAULT_NUM_BUCKETS;
  result->num_elements         = 0;
  return result;
}

void DestroyBinopPrecedenceTable(BinopPrecedenceTable* BPtbl)
{
  if (BPtbl == NULL)
    return;
  else if (BPtbl->table == NULL)
  {
    free(BPtbl);
    return;
  }


  for (int i = 0; i < BPtbl->num_buckets; i++)
  {
    // cur(sor|rent)
    BPElem* cur = BPtbl->table[i], *prv;
    while (cur != NULL)
    {
      prv = cur;
      cur = cur->next;
      free(prv);
    }
  }
  BPtbl->table = NULL;
  BPtbl->num_buckets = 0;
  BPtbl->num_elements = 0;
  free(BPtbl);
}

BinopPrecAssoc* FindBinopPrecAssoc(BinopPrecedenceTable* BPtbl, InternedString op)
{
  if (BPtbl == NULL || BPtbl->table == NULL)
    return (BinopPrecAssoc*)NULL;

  int hash = (int)(op);
  hash %= BPtbl->num_buckets;

  BPElem *cur = BPtbl->table[hash];

  if (cur != NULL)
  {
    do {
      if (cur->data.op == op)
        return &(cur->data);
      else
        cur = cur->next;
    } while (cur != NULL);
  }

  return (BinopPrecAssoc*)NULL;
}

BinopPrecAssoc* InsertBinopPrecAssoc(BinopPrecedenceTable* BPtbl, InternedString op, int prec, Associativity assoc)
{
  if (BPtbl == NULL || BPtbl->table == NULL)
    return (BinopPrecAssoc*)NULL;

  int hash = (int)(op);
  hash %= BPtbl->num_buckets;

  BPElem *head = BPtbl->table[hash];

  BPElem* elem = (BPElem*)malloc(sizeof(BPElem));
  elem->data.op = op;
  elem->data.precedence = prec;
  elem->data.associativity = assoc;
  elem->next = head;
  BPtbl->table[hash] = elem; // prepend the new node to the list.
  BPtbl->num_elements++;
  return &(elem->data);
}
