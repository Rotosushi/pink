#pragma once
#include <memory>

#include "llvm/IR/DataLayout.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"

#include "StringInterner.hpp"
#include "SymbolTable.hpp"
#include "BinopSet.hpp"
#include "UnopSet.hpp"

/*
  Environment is the set of data that is shared
  between most of the parts of the other computations
  available within the program. this data is required
  to be available to be referenced and modified by
  different subalgorithms during their work.
  the sharing nature here, is the avenue of communication
  between the algorithms such that each algorithm doesn't need
  to keep track of state persay. (we don't need excel style dynamic
  variable updating, for instance)
  each algorithm instead adds to and is written to query from
  a central source of truth. a reference to these sources of
  thruth is what we store here.

*/
class Environment
{
private:
  // instead of making this variable a static local
  // of the gensym procedure, i'm storing it in the
  // Environment, which already does the, singleton
  // variable shared between calls of the same procedure
  // thing, that needs to happen between calls of gensym.
  // (and everything else in the compiler.)
  size_t                             lambda_count;
  // interning is so cool!
  std::shared_ptr<StringInterner>    interned_names;
  std::shared_ptr<StringInterner>    interned_operators;
  // symboltable is a DenseMap of InternedStrings to shared_ptr<Ast>s
  std::shared_ptr<SymbolTable>       symbols;
  // in fact all *Table's are DenseMaps.
  std::shared_ptr<BinopTable>          binops;
  std::shared_ptr<UnopTable>           unops;
  // and then the llvm stuff that we need.
  llvm::StringRef                    target_triple;
  std::shared_ptr<llvm::DataLayout>  data_layout;
  std::shared_ptr<llvm::LLVMContext> context;
  std::shared_ptr<llvm::Module>      module;
  std::shared_ptr<llvm::IRBuilder>   irbuilder;

public:
  Environment(std::shared_ptr<StringInterner>    interned_names,
              std::shared_ptr<StringInterner>    interned_operators,
              std::shared_ptr<SymbolTable>       symbols,
              std::shared_ptr<BinopSet>          binops,
              std::shared_ptr<UnopSet>           unops,
              std::shared_ptr<llvm::LLVMContext> context,
              std::shared_ptr<llvm::Module>      module,
              std::shared_ptr<llvm::IRBuilder>   irbuilder);

  size_t&            GetLambdaCount();
  StringInterner&    GetInternedNames();
  StringInterner&    GetInternedOperators();
  SymbolTable&       GetBoundSymbols();
  BinopSet&          GetBoundBinops();
  UnopSet&           GetBoundUnops();
  llvm::LLVMContext& GetContext();
  llvm::Module&      GetModule();
  llvm::IRBuilder&   GetIRBuilder();
};
