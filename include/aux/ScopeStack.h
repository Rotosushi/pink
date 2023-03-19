// Copyright (C) 2023 cadence
//
// This file is part of pink.
//
// pink is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// pink is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with pink.  If not, see <http://www.gnu.org/licenses/>.

#pragma once
#include <list>
#include <stack>

#include "llvm/IR/Value.h"

#include "type/Type.h"

#include "aux/Map.h"
#include "aux/StringInterner.h"

namespace pink {
class ScopeStack {
public:
  using Key   = InternedString;
  using Value = std::pair<Type::Pointer, llvm::Value *>;
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
    auto Type() noexcept -> Type::Pointer { return element.Value().first; }
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
    for (auto &scope : stack) {
      auto found = scope.Lookup(name);
      if (found) {
        return found;
      }
    }
    return {};
  }

  auto LookupLocal(InternedString name) -> std::optional<Symbol> {
    return stack.front().Lookup(name);
  }

  void Bind(InternedString name, Type::Pointer type, llvm::Value *value) {
    stack.front().Register(name, {type, value});
  }
};
} // namespace pink
