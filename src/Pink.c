/*

  There is no such thing as a hard problem,
   only problems which have yet to be
   deconstructed into the proper series of
   easy problems.


  Failure is just another known path we take.

  Failure is just as important as Success.






  something i am noticing from this
  rewrite, much of the benefiets that
  come from c++ can be had by simply
  sticking to a few choices when writing
  the code. breaking up the switch bodies
  into separate procedures defined within
  the file, alongside the definitions of the
  structure representing the idea in memory.
  to evoke the idea of OOP.
*/

#include "Ast.h"
#include "StringInterner.h"
#include "TypeInterner.h"
#include "SymbolTable.h"
#include "BinopPrecedence.h"
#include "BinopTable.h"
#include "UnopTable.h"
#include "Parser.h"

#include "PinkKernel.h"
#include "Repl.h"

#include "Environment.h"

// main should look something like this for
// the interpretive setup.
int main (int argc, char** argv)
{
  StringInterner*       interned_ids     = CreateStringInterner();
  StringInterner*       interned_ops     = CreateStringInterner();
  TypeInterner*         interned_types   = CreateTypeInterner();
  SymbolTable*          global_scope     = CreateSymbolTable((SymbolTable*)NULL);
  BinopPrecedenceTable* precedence_table = CreateBinopPrecedenceTable();
  BinopTable*           binops           = CreateBinopTable();
  UnopTable*            unops            = CreateUnopTable();
  Parser*               parser           = CreateParser(global_scope, interned_ids, interned_ops, interned_types, precedence_table, binops, unops);
  Environment*   environment             = (Environment*)malloc(sizeof(Environment));
  environment->parser      = parser;
  environment->outer_scope = global_scope;
  environment->interned_ids = interned_ids;
  environment->interned_ops = interned_ops;
  environment->interned_types = interned_types;
  environment->precedence_table = precedence_table;
  environment->binops = binops;
  environment->unops  = unops;

  RegisterPrimitiveBinops(environment);
  RegisterPrimitiveUnops(environment);

  Repl(stdin, stdout, environment);


  return 0;
}
