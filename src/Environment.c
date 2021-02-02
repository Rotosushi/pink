#include <string.h>
#include <stdlib.h>

#include "SymbolTable.h"
#include "StringInterner.h"
#include "Parser.h"
#include "BinopTable.h"
#include "BinopPrecedence.h"
#include "UnopTable.h"
#include "Environment.h"


Environment* CreateEnvironment(Parser* parser,
                               SymbolTable* outer_scope,
                               StringInterner* Iids,
                               StringInterner* Iops,
                               TypeInterner* Ity,
                               BinopPrecedenceTable* BPAtbl,
                               BinopTable* bop,
                               UnopTable*  uop)
{
  Environment* result = (Environment*)malloc(sizeof(Environment));
  result->parser = parser;
  result->outer_scope = outer_scope;
  result->interned_ids = Iids;
  result->interned_ops = Iops;
  result->precedence_table = BPAtbl;
  result->binops = bop;
  result->unops = uop;
  return result;
}

void DestroyEnvironment(Environment* env)
{
  free(env);
}
