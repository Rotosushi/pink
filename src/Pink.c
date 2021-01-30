/*
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
#include "SymbolTable.h"
#include "BinopPrecedence.h"
#include "BinopTable.h"
#include "UnopTable.h"


// main should look something like this for
// the interpretive setup.
int main (int argc, char** argv)
{
  StringInterner*       interned_ids     = CreateStringInterner();
  StringInterner*       interned_ops     = CreateStringInterner();
  SymbolTable*          global_scope     = CreateSymbolTable((SymbolTable*)NULL);
  BinopPrecedenceTable* precedence_table = CreateBinopPrecedenceTable();
  BinopTable*           binops           = CreateBinopTable();
  UnopTable*            unops            = CreateUnopTable();
  Parser*               parser           = CreateParser(interned_ids, interned_ops, precedence_table, binops, unops);
  Environment*          environment      = CreateEnvironemnt(parser, interned_ids, interned_ops, precedence_table, binops, unops);

  RegisterPrimitiveBinops(environment);
  RegisterPrimitiveUnops(environment);

  //cout << "Welcome to Pink! v0.0.2\n"
  //     << "press ctrl+c to exit.\n";

  Repl(stdin, stdout, environment);


  return 0;
}
