#pragma once
#include <memory>

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"

#include "StringInterner.hpp"
#include "SymbolTable.hpp"
#include "BinopSet.hpp"
#include "UnopSet.hpp"


class Environment
{
private
  std::shared_ptr<StringInterner>    interned_names;
  std::shared_ptr<StringInterner>    interned_operators;
  std::shared_ptr<SymbolTable>       symbols;
  std::shared_ptr<BinopSet>          binops;
  std::shared_ptr<UnopSet>           unops;
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

  StringInterner&    GetInternedNames();
  StringInterner&    GetInternedOperators();
  SymbolTable&       GetBoundSymbols();
  BinopSet&          GetBoundBinops();
  UnopSet&           GetBoundUnops();
  llvm::LLVMContext& GetContext();
  llvm::Module&      GetModule();
  llvm::IRBuilder&   GetIRBuilder();
};
