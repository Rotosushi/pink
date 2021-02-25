#ifndef BINOPPRECEDENCE_H
#define BINOPPRECEDENCE_H

#include "StringInterner.h"
#include "Associativity.h"

typedef struct BinopPrecAssoc
{
  InternedString op;
  int precedence;
  Associativity associativity;
} BinopPrecAssoc;

typedef struct BPElem
{
  BinopPrecAssoc data;
  struct BPElem* next;
} BPElem;

typedef struct BinopPrecedenceTable
{
  BPElem** table;
  int num_buckets;
  int num_elements;
} BinopPrecedenceTable;

BinopPrecedenceTable* CreateAstBinopPrecedenceTable();
void                  DestroyBinopPrecedenceTable(BinopPrecedenceTable* BPtbl);
BinopPrecAssoc* FindBinopPrecAssoc(BinopPrecedenceTable* BPtbl, InternedString op);
BinopPrecAssoc* InsertBinopPrecAssoc(BinopPrecedenceTable* BPtbl, InternedString op, int prec, Associativity assoc);

#endif // !BINOPPRECEDENCE_H
