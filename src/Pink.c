/*

  There is no such thing as a hard problem,
   only problems which have yet to be
   deconstructed into the proper series of
   easy problems.


  Failure is just another known path we take.

  Failure is just as important as Success.




  execution is innermost first, and strict.

  because that is how the information flows
  from the perspective of C.

  (and from most assembly langauges as well)

  also, destination is on the left.
  just because, and consistency is important.
  so, there, it's decided.


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
  ScopeSet              global_scope     = 1;
  StringInterner*       interned_ids     = CreateStringInterner();
  StringInterner*       interned_ops     = CreateStringInterner();
  TypeInterner*         interned_types   = CreateTypeInterner();
  SymbolTable*          global_symbols   = CreateSymbolTable((SymbolTable*)NULL);
  BinopPrecedenceTable* precedence_table = CreateAstBinopPrecedenceTable();
  BinopTable*           binops           = CreateAstBinopTable();
  UnopTable*            unops            = CreateUnopTable();
  Parser*               parser           = CreateParser(global_symbols, global_scope, interned_ids, interned_ops, interned_types, precedence_table, binops, unops);
  Environment*          environment      = CreateEnvironment(parser, global_symbols, global_scope, interned_ids, interned_ops, interned_types, precedence_table, binops, unops);

  RegisterPrimitiveBinops(environment);
  RegisterPrimitiveUnops(environment);

  Repl(stdin, stdout, environment);

  return 0;
}
