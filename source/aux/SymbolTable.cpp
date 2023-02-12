#include <utility>

#include "aux/SymbolTable.h"

namespace pink {
auto SymbolTable::Lookup(InternedString symbol) const -> std::optional<Value> {
  auto iter = map.find(symbol);
  if (iter == map.end()) {
    // attempt to find the symbol in the outer_scope scope.
    if (outer_scope != nullptr) {
      return outer_scope->Lookup(symbol);
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

void SymbolTable::Unbind(InternedString symbol) {
  auto iter = map.find(symbol);

  if (iter == map.end()) {
    return;
  }

  map.erase(iter);
}
} // namespace pink
