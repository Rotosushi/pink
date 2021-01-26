#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

// parser
// current scope
// binop precedences set
// binop eliminator set
// unop eliminator set
// cleanup list (only when sequence is added)
#include "StringInterner.h"
#include "BinopTable.h"
#include "BinopPrecedence.h"
#include "UnopTable.h"

struct Parser;

typedef struct Environment
{
  struct Parser*               parser;
  StringInterner*       interned_ids;
  StringInterner*       interned_ops;
  BinopPrecedenceTable* precedence_table;
  BinopTable*             binops;
  UnopTable*              unops;
} Environment;

Environment* CreateEnvironment(struct Parser* parser,
                               StringInterner* Iids,
                               StringInterner* Iops,
                               BinopPrecedenceTable* BPAtbl,
                               BinopTable* bop,
                               UnopTable*  uop);

void DestroyEnvironment(Environment* env); // even though this is a single call to free()
                                           // it's worth it for the horizontal consistency.
#endif // !ENVIRONMENT_H
