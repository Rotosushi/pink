#include <string.h>
#include <stdlib.h>

#include "StringInterner.h"
#include "BinopSet.h"
#include "BinopPrecedenceTable.h"
#include "UnopSet.h"
#include "Environment.h"


Environment* CreateEnvironment(Parser* parser,
                               StringInterner* Iids,
                               StringInterner* Iops,
                               BinopPrecedenceTable* BPAtbl,
                               BinopSet* bop,
                               UnopSet*  uop)
{
  Environment* result = (Environement*)malloc(sizeof(Environment));
  result = {parser, Iids, Iops, BPAtbl, bop, uop};
  return result;
}

void DestroyEnvironment(Environment* env)
{
  free(env);
}
