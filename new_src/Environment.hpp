#pragma once

// std library code
#include <string>
#include <memory>

// llvm code
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"

#include "Parser.hpp"

// my code
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
  truth is what we store here. making it so these sources
  of truth are not globally allocated makes updating the program
  later easier. this is part of why globals are to be avoided,
  and this is an instance of a techinque avoiding global variables,
  while still getting the benefiets.

*/

namespace pink {

class Environment
{


public:
  const Parser&              parser;
  // interning is so cool!
  const StringInterner&      interned_names;
  const StringInterner&      interned_operators;
  // symboltable is a DenseMap of InternedStrings to shared_ptr<Ast>s
  const SymbolTable&         symbols;
  // in fact all *Table's are DenseMaps.
  const BinopTable&          binops;
  const BinopPrecedenceTable binop_precedence_table;
  const UnopTable&           unops;
  // and then the llvm stuff that we need.
  const std::string&         target_triple;
  const llvm::DataLayout&    data_layout;
  const llvm::LLVMContext&   context;
  const llvm::Module&        module;
  const llvm::IRBuilder&     builder;

private:
  llvm::Value* BuildLoadAggregate(llvm::Value* pointer, llvm::Type* allocated_type);

  void BuildStoreAggregate(llvm::Value* pointer, llvm::Constant* value);
public:
  Environment(const Parser&               parser,
              const StringInterner&       interned_names,
              const StringInterner&       interned_operators,
              const SymbolTable&          symbols,
              const BinopTable&           binops,
              const BinopPrecedenceTable& bpt,
              const UnopTable&            unops,
              const std::string&          target_triple,
              const llvm::DataLayout&     data_layout,
              const llvm::LLVMContext&    context,
              const llvm::Module&         module,
              const llvm::IRBuilder&      builder);

  // general helper routines
  llvm::IntegerType* GetNilType();
  llvm::IntegerType* GetIntegerType();
  llvm::IntegerType* GetBooleanType();

  Judgement BuildZeroedValue(std::shared_ptr<Ast> type);


  // codegen helper routines.
  // builds an allocation for the passed value
  // returns the allocation point of the
  // variable, i'm fairly sure this is a
  // Pointer to the Alloca (the local stack of the fn)
  llvm::AllocaInst* BuildLocalAlloca(llvm::Type* type, std::string& name, llvm::Function* fn);

  // loads the value inside the alloca into a llvm::Value*
  // this handles the case where the value is too big
  // to fit inside a single load instruction.
  llvm::Constant* BuildLoad(llvm::AllocaInst* alloc);

  // builds enough store instructions to copy the whole
  // constant into the alloca. FatalError if the alloca isn't
  // big enough.
  void BuildStore(llvm::AllocaInst* pointer, llvm::Constant* value);
};

}
