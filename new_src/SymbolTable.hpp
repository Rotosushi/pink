#pragma once
#include <memory>
#include <optional>

#include "llvm/ADT/DenseMap.h"

#include "Ast.hpp"
#include "ScopeSet.hpp"
#include "StringInterner.hpp"

class SymbolTable
{
private:
  // we use a dense map here to be speedy.
  // and because we are mapping a pointer to
  // another pointer.
  llvm::DenseMap<InternedString, std::shared_ptr<Ast>> map;
  SymbolTable* outer;
  ScopeSet     scope;

  // used to do lookup recursively.
  std::optional<Ast*> LookupHelper(InternedString target_name, ScopeSet target_scope, std::shared_ptr<Ast> best_match, ScopeSet best_scope);
public:
  SymbolTable();
  SymbolTable(SymbolTable* outer);

  std::optional<std::shared_ptr<Ast>> Lookup(InternedString name, ScopeSet scope); // recursive lookup
  std::optional<std::shared_ptr<Ast>> LookupLocal(InternedString name); // non-recursive lookup.

  void Bind(InternedString name, std::shared_ptr<Ast> value);
  void Unbind(InternedString name);
};
