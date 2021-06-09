#pragma once
#include <string>
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
  // interning is so cool!
  const StringInterner&    interned_names;
  const StringInterner&    interned_operators;
  // symboltable is a DenseMap of InternedStrings to shared_ptr<Ast>s
  const SymbolTable&       symbols;
  // in fact all *Table's are DenseMaps.
  const BinopTable&        binops;
  const UnopTable&         unops;
  // and then the llvm stuff that we need.
  const std::string&       target_triple;
  const llvm::DataLayout&  data_layout;
  const llvm::LLVMContext& context;
  const llvm::Module&      module;
  const llvm::IRBuilder&   builder;

  llvm::Value* BuildLoadAggregate(llvm::Value* pointer, llvm::Type* allocated_type);

  void BuildStoreAggregate(llvm::Value* pointer, llvm::Constant* value);
public:
  Environment(const StringInterner&    interned_names,
              const StringInterner&    interned_operators,
              const SymbolTable&       symbols,
              const BinopTable&        binops,
              const UnopTable&         unops,
              const std::string&       target_triple,
              const llvm::DataLayout&  data_layout,
              const llvm::LLVMContext& context,
              const llvm::Module&      module,
              const llvm::IRBuilder&   builder);

  const StringInterner&    GetInternedNames();
  const StringInterner&    GetInternedOperators();
  const SymbolTable&       GetSymbolTable();
  const BinopSet&          GetBinopTable();
  const UnopSet&           GetUnopTable();
  const std::string&       GetTargetTriple();
  const llvm::DataLayout&  GetDataLayout();
  const llvm::LLVMContext& GetContext();
  const llvm::Module&      GetModule();
  const llvm::IRBuilder&   GetIRBuilder();

  // builds an allocation for the passed value
  // returns the allocation point of the
  // variable, i'm fairly sure this is a
  // Pointer to the Alloca (the local stack of the fn)
  llvm::AllocaInst* BuildLocalAlloca(llvm::Type* type, std::string& name, llvm::Function* fn);

  // loads the value inside the alloca into a llvm::Value*
  // this handles the case where the value is too big
  // to fit inside a single load instruction.
  llvm::Constant* BuildLoad(llvm::AllocaInst* alloc);

  void BuildStore(llvm::AllocaInst* pointer, llvm::Constant* value);
};
