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

/**
 * @file UnopTable.h
 * @brief Header for class UnopTable
 * @version 0.1
 */
#pragma once
#include <vector>

#include "llvm/IR/Value.h" // llvm::Value

#include "type/Type.h" // pink::Type

#include "aux/Error.h" // pink::Error
/*
  so, types ~must~ be interned using StrictEquality
  which means that Type::Pointer address equality
  also models StrictEquality. Which adds a problem
  to the way we handle operator overload sets.
  more specifically how we handle lookup. up until
  now the lookup has been fine using pointer equality,
  and thus the problem could be reduced into
  a map (token -> overload set) of maps (type -> overload).
  however, due to StrictEquality a const T != T.
  so, even though T == T, and the operator works with T,
  because we defined the operator to take const T (or T)
  then lookup fails when using a T (or const T). in fact
  this problem exists for each flag we choose to track within
  a type. so,
  A) we can insert an overload for each kind of flag present in the Map.
  this means 2 overloads per flag state, which means 2^(the number of flags)
  overloads, per Type T we want the operator to handle. the has a benefit of
  keeping the map structure, and a downside of implementing a new
  binop or a new type onto an existing binop take a lot of boilerplate code.
  another upside is that we can choose to not implement an overload if the
  particular combination of flags means the operator cannot work.

  B) we can use a relaxed form of equality to perform lookup. that does not
    care about the state of the flags. then T == const T == other-flag T == ...
    then we get to use the same implementation for all possible variants
    of T. this has a benefit of scaling implicitly no matter if we add
    more flags later. and a downside of losing the abiltiy to use a traditional,
    map implementation. a downside of this approach is that there is no way to
    express that one overload for a given type and combination of flags
    does not work. for instance Address of cannot be implemented given a
    literal value, as they do not reside within the emitted program.


*/
#include "aux/Map.h" // pink::Map<K, V>

#include "front/Token.h" // pink::Token

namespace pink {
class CompilationUnit;

class UnopCodegen {
public:
  /**
   * @brief pointer to a function which can be used
   * to generate an llvm IR implementation of a given unop
   */
  using Function = llvm::Value *(*)(llvm::Value *term, CompilationUnit &env);

private:
  Type::Pointer return_type;
  Function      function;

public:
  UnopCodegen() noexcept
      : return_type{nullptr},
        function{nullptr} {}
  UnopCodegen(Type::Pointer return_type, Function function) noexcept
      : return_type{return_type},
        function{function} {}
  ~UnopCodegen() noexcept                                            = default;
  UnopCodegen(const UnopCodegen &other) noexcept                     = default;
  UnopCodegen(UnopCodegen &&other) noexcept                          = default;
  auto operator=(const UnopCodegen &other) noexcept -> UnopCodegen & = default;
  auto operator=(UnopCodegen &&other) noexcept -> UnopCodegen      & = default;

  [[nodiscard]] auto GetReturnType() const noexcept -> Type::Pointer {
    return return_type;
  }

  [[nodiscard]] auto operator()(llvm::Value     *value,
                                CompilationUnit &env) const noexcept
      -> llvm::Value * {
    return function(value, env);
  }
};

class UnopOverloadSet {
public:
  using Key       = Type::Pointer;
  using Value     = UnopCodegen;
  using Overloads = std::vector<std::pair<Key, Value>>;

  class Overload {
  private:
    Overloads::iterator literal;

  public:
    Overload(Overloads::iterator element) noexcept
        : literal{element} {}
    [[nodiscard]] auto ArgumentType() const noexcept -> Key {
      return literal->first;
    }
    [[nodiscard]] auto ReturnType() const noexcept -> Type::Pointer {
      return literal->second.GetReturnType();
    }
    [[nodiscard]] auto operator()(llvm::Value     *value,
                                  CompilationUnit &env) const noexcept
        -> llvm::Value * {
      return literal->second(value, env);
    }
  };

private:
  Overloads overloads;

public:
  UnopOverloadSet() noexcept                             = default;
  virtual ~UnopOverloadSet() noexcept                    = default;
  UnopOverloadSet(const UnopOverloadSet &other) noexcept = delete;
  UnopOverloadSet(UnopOverloadSet &&other) noexcept      = default;
  auto operator=(const UnopOverloadSet &other) noexcept
      -> UnopOverloadSet & = delete;
  auto operator=(UnopOverloadSet &&other) noexcept
      -> UnopOverloadSet & = default;

  [[nodiscard]] auto Empty() const noexcept -> bool {
    return overloads.empty();
  }

  auto Register(Type::Pointer         argument_type,
                Type::Pointer         return_type,
                UnopCodegen::Function generator) -> Overload {
    auto found = Lookup(argument_type);
    if (found) {
      return found.value();
    }

    overloads.emplace_back(argument_type, UnopCodegen(return_type, generator));
    return std::prev(overloads.end());
  }

  auto Lookup(Type::Pointer argument_type) -> std::optional<Overload> {
    auto cursor = overloads.begin();
    auto end    = overloads.end();
    while (cursor != end) {
      if (Equals(argument_type, cursor->first)) {
        return cursor;
      }
      cursor++;
    }
    return {};
  }
};

/**
 * @brief Represents the set of all known unary operators
 */
class UnopTable {
public:
  using Key   = Token;
  using Value = std::unique_ptr<UnopOverloadSet>;
  using Table = Map<Key, Value>;

  class Unop {
  private:
    Table::Element element;

  public:
    Unop(Table::Element element) noexcept
        : element(std::move(element)) {}
    [[nodiscard]] auto Empty() const noexcept -> bool {
      return element.Value()->Empty();
    }
    auto Register(Type::Pointer         argument_type,
                  Type::Pointer         return_type,
                  UnopCodegen::Function generator)
        -> UnopOverloadSet::Overload {
      return element.Value()->Register(argument_type, return_type, generator);
    }
    [[nodiscard]] auto Lookup(Type::Pointer argument_type)
        -> std::optional<UnopOverloadSet::Overload> {
      return element.Value()->Lookup(argument_type);
    }
  };

private:
  Table table;

public:
  UnopTable() noexcept                                           = default;
  ~UnopTable() noexcept                                          = default;
  UnopTable(const UnopTable &other) noexcept                     = delete;
  UnopTable(UnopTable &&other) noexcept                          = default;
  auto operator=(const UnopTable &other) noexcept -> UnopTable & = delete;
  auto operator=(UnopTable &&other) noexcept -> UnopTable      & = default;

  // NOLINTNEXTLINE(readability-identifier-length)
  auto RegisterUnop(Token                 op,
                    Type::Pointer         argument_type,
                    Type::Pointer         return_type,
                    UnopCodegen::Function generator) -> Unop {
    assert(argument_type != nullptr);
    assert(return_type != nullptr);
    assert(generator != nullptr);
    auto found = Lookup(op);
    if (found) {
      found->Register(argument_type, return_type, generator);
      return found.value();
    }

    Unop unop = table.Register(op, std::make_unique<UnopOverloadSet>());
    unop.Register(argument_type, return_type, generator);
    return unop;
  }

  // NOLINTNEXTLINE(readability-identifier-length)
  auto Lookup(Token op) -> std::optional<Unop> { return table.Lookup(op); }
};
} // namespace pink
