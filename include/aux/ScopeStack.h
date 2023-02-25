#pragma once
#include <list>
#include <stack>

#include "aux/SymbolTable.h"

namespace pink {
class ScopeStack {
public:
  using Scope = SymbolTable;
  using Stack = std::stack<Scope, std::list<Scope>>;

private:
  Stack stack;

public:
  ScopeStack() { stack.emplace(); }
  ~ScopeStack()                                           = default;
  ScopeStack(const ScopeStack &other)                     = delete;
  ScopeStack(ScopeStack &&other)                          = default;
  auto operator=(const ScopeStack &other) -> ScopeStack & = delete;
  auto operator=(ScopeStack &&other) -> ScopeStack      & = default;

  void Reset() {
    while (!stack.empty()) {
      stack.pop();
    }
    stack.emplace();
  }
  void PushScope() { stack.emplace(&stack.top()); }
  void PopScope() { stack.pop(); }

  auto Lookup(InternedString symbol) const
      -> std::optional<SymbolTable::Value> {
    return stack.top().Lookup(symbol);
  }
  auto LookupLocal(InternedString symbol) const
      -> std::optional<SymbolTable::Value> {
    return stack.top().LookupLocal(symbol);
  }
  void Bind(InternedString symbol, Type::Pointer type, llvm::Value *value) {
    stack.top().Bind(symbol, type, value);
  }
};
} // namespace pink
