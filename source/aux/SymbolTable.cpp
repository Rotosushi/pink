#include <utility>

#include "aux/SymbolTable.h"

namespace pink {
SymbolTable::SymbolTable()
    : outer(nullptr) {}

SymbolTable::SymbolTable(SymbolTable *outer_scope)
    : outer(outer_scope) {}

auto SymbolTable::OuterScope() const -> SymbolTable * { return outer; }

auto SymbolTable::IsGlobal() const -> bool { return outer == nullptr; }

auto SymbolTable::Lookup(InternedString symbol) const -> std::optional<Value> {
  auto iter = map.find(symbol);
  if (iter == map.end()) {
    // attempt to find the symbol in the outer scope.
    if (outer != nullptr) {
      return outer->Lookup(symbol);
    }
    // This is the global scope, if it isn't here, it isn't bound.
    return {};
  }
  return {iter->second};
}

auto SymbolTable::LookupLocal(InternedString symbol) const
    -> std::optional<Value> {
  auto iter = map.find(symbol);
  if (iter == map.end()) {
    return {};
  }
  return {iter->second};
}

void SymbolTable::Bind(InternedString symbol,
                       Type::Pointer  type,
                       llvm::Value   *term) {
  map.insert(std::make_pair(symbol, std::make_pair(type, term)));
}

void SymbolTable::Unbind(InternedString symbol) {
  auto iter = map.find(symbol);

  if (iter == map.end()) {
    return;
  }

  map.erase(iter);
}
} // namespace pink
