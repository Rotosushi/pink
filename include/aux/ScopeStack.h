#pragma once
#include <list>
#include <stack>

#include "llvm/IR/Value.h"

#include "type/TypeInterface.h"

#include "aux/Map.h"
#include "aux/StringInterner.h"

namespace pink {
class ScopeStack {
public:
  using Key   = InternedString;
  using Value = std::pair<TypeInterface::Pointer, llvm::Value *>;
  using Scope = Map<Key, Value>;
  using Stack = std::list<Scope>;

  class Symbol {
  private:
    Scope::Element element;

  public:
    Symbol(Scope::Element element) noexcept
        : element(element) {}
    ~Symbol() noexcept                                         = default;
    Symbol(const Symbol &element) noexcept                     = default;
    Symbol(Symbol &&element) noexcept                          = default;
    auto operator=(const Symbol &element) noexcept -> Symbol & = default;
    auto operator=(Symbol &&element) noexcept -> Symbol      & = default;

    auto Name() noexcept -> InternedString { return element.Key(); }
    auto Type() noexcept -> TypeInterface::Pointer {
      return element.Value().first;
    }
    auto Value() noexcept -> llvm::Value * { return element.Value().second; }
  };

private:
  Stack stack;

public:
  ScopeStack() { stack.emplace_front(); }
  ~ScopeStack()                                           = default;
  ScopeStack(const ScopeStack &other)                     = delete;
  ScopeStack(ScopeStack &&other)                          = default;
  auto operator=(const ScopeStack &other) -> ScopeStack & = delete;
  auto operator=(ScopeStack &&other) -> ScopeStack      & = default;

  [[nodiscard]] auto IsGlobal() const noexcept -> bool {
    return stack.size() == 1;
  }
  void Reset() {
    stack.clear();
    stack.emplace_front();
  }
  void PushScope() { stack.emplace_front(); }
  void PopScope() { stack.pop_front(); }

  auto Lookup(InternedString name) -> std::optional<Symbol> {
    auto found = stack.front().Lookup(name);

    if (found) {
      return {found.value()};
    }

    for (auto &scope : stack) {
      found = scope.Lookup(name);
      if (found) {
        return found;
      }
    }

    return {};
  }

  auto LookupLocal(InternedString name) -> std::optional<Symbol> {
    return stack.front().Lookup(name);
  }

  void
  Bind(InternedString name, TypeInterface::Pointer type, llvm::Value *value) {
    stack.front().Register(name, {type, value});
  }
};
} // namespace pink
