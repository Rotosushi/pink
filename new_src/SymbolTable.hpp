#pragma once
#include <memory>
#include <optional>

#include "llvm/ADT/DenseMap.h"

#include "llvm/IR/Instructions.h"

#include "Ast.hpp"
#include "ScopeSet.hpp"
#include "StringInterner.hpp"

class SymbolTable
{
private:
  // we use a dense map here to be speedy.
  // and because we are mapping a pointer to
  // another pointer. so the types are super small.
  // okay, if this was modified to hold a AllocaInst*
  // directly we can no longer leverage polymorphism
  // to bind names to types during typing and values during codegen.
  llvm::DenseMap<InternedString, llvm::AllocaInst*> map;
  SymbolTable* outer;
  ScopeSet     scope;

  // used to do lookup recursively.
  std::optional<llvm::AllocaInst*> LookupHelper(InternedString target_name, ScopeSet target_scope, llvm::AllocaInst* best_match, ScopeSet best_scope);
public:
  SymbolTable();
  SymbolTable(SymbolTable* outer);

  std::string  Gensym(std::string& prefix = "");
  bool         IsGlobalScope();
  SymbolTable* GetParentSymbolTable();

  std::optional<llvm::AllocaInst*> Lookup(InternedString name, ScopeSet scope); // recursive lookup
  std::optional<llvm::AllocaInst*> LookupLocal(InternedString name); // non-recursive lookup.

  void Bind(InternedString name, llvm::AllocaInst*);
  void Unbind(InternedString name);
};
