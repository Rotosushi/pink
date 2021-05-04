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

#include "llvm-c/Core.h"

#include "Ast.h"
#include "StringInterner.hpp"
#include "TypeInterner.h"
#include "SymbolTable.h"
#include "BinopPrecedence.h"
#include "BinopTable.h"
#include "UnopTable.h"
#include "Parser.h"

#include "PinkKernel.h"
#include "Repl.h"

#include "Environment.h"


int main (int argc, char** argv)
{
  // so, technically i can use the statically
  // allocated 'global' llvm context. however,
  // i like not having to really worry about threading.
  // and this forces me into the style that is needed to
  // run multiple LLVM contexts accross multiple threads.
  LLVMContextRef llvm_context = LLVMContextCreate();
  LLVMModuleRef  pink = LLVMModuleCreateWithNameInContext("Pink", llvm_context);
  LLVMBuilderRef llvm_instruction_builder = LLVMCreateBuilderInContext(llvm_context);


  ScopeSet              global_scope     = 1;
  StringInterner*       interned_ids     = CreateStringInterner();
  StringInterner*       interned_ops     = CreateStringInterner();
  TypeInterner*         interned_types   = CreateTypeInterner();
  SymbolTable*          global_symbols   = CreateSymbolTable((SymbolTable*)NULL);
  BinopPrecedenceTable* precedence_table = CreateBinopPrecedenceTable();
  BinopTable*           binops           = CreateBinopTable();
  UnopTable*            unops            = CreateUnopTable();
  Parser*               parser           = CreateParser(global_symbols, global_scope, interned_ids, interned_ops, interned_types, precedence_table, binops, unops);
  Environment*          environment      = CreateEnvironment(parser, global_symbols, global_scope, interned_ids, interned_ops, interned_types, precedence_table, binops, unops);

  RegisterPrimitiveBinops(environment);
  RegisterPrimitiveUnops(environment);

  Repl(stdin, stdout, environment);


  DestroyStringInterner(interned_ids);
  DestroyStringInterner(interned_ops);
  DestroyTypeInterner(interned_types);
  DestroySymbolTable(global_symbols);
  DestroyBinopPrecedenceTable(precedence_table);
  DestroyBinopTable(binops);
  DestroyUnopTable(unops);
  DestroyParser(parser);
  DestroyEnvironment(environment);

  LLVMDisposeBuilder(llvm_instruction_builder);
  LLVMModuleDispose(pink);
  LLVMContextDispose(llvm_context);
  return 0;
}
