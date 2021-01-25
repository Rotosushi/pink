#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

// parser
// current scope
// binop precedences set
// binop eliminator set
// unop eliminator set
// cleanup list (only when sequence is added)
#include "StringInterner.h"
#include "BinopSet.h"
#include "BinopPrecedenceTable.h"
#include "UnopSet.h"

typedef struct Environment
{
  Parser*               parser;
  StringInterner*       interned_ids;
  StringInterner*       interned_ops;
  BinopPrecedenceTable* precedence_table;
  BinopSet*             binops;
  UnopSet*              unops;
} Environment;

Environment* CreateEnvironment(Parser* parser,
                               StringInterner* Iids,
                               StringInterner* Iops,
                               BinopPrecedenceTable* BPAtbl,
                               BinopSet* bop,
                               UnopSet*  uop);

void DestroyEnvironment(Environment* env); // even though this is a single call to free()
                                           // it's worth it for the horizontal consistency.
#endif // !ENVIRONMENT_H
