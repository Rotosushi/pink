#include <algorithm>
#include <utility>

#include "aux/SymbolTable.h"

namespace pink {
auto SymbolTable::Lookup(InternedString symbol) const -> std::optional<Value> {
  auto iterator =
      std::find_if(table.begin(), table.end(), [symbol](const auto &binding) {
        return symbol == std::get<InternedString>(binding);
      });
  if (iterator == table.end()) {
    // attempt to find the symbol in the outer_scope scope.
    if (outer_scope != nullptr) {
      return outer_scope->Lookup(symbol);
    }
    // This is the global scope, if it isn't here, it isn't bound.
    return {};
  }
  return {*iterator};
}

auto SymbolTable::LookupLocal(InternedString symbol) const
    -> std::optional<Value> {
  auto iterator =
      std::find_if(table.begin(), table.end(), [symbol](const auto &binding) {
        return symbol == std::get<InternedString>(binding);
      });
  if (iterator == table.end()) {
    return {};
  }
  return {*iterator};
}
} // namespace pink
