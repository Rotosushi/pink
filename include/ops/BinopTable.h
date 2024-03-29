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
 * @file BinopTable.h
 * @brief Header for class BinopTable
 * @version 0.1
 */
#pragma once
#include <optional> // std::optional
#include <utility>  // std::pair<>
#include <vector>   // std::vector

#include "llvm/IR/Value.h"

#include "aux/Map.h"

#include "type/Type.h"

#include "front/Token.h"

#include "ops/PrecedenceAndAssociativity.h"

namespace pink {
class CompilationUnit;

/**
 * @brief Represents an implementation of a particular Binop
 */
class BinopCodegen {
public:
  using Function = llvm::Value *(*)(llvm::Value     *left,
                                    llvm::Value     *right,
                                    CompilationUnit &env);

private:
  Type::Pointer return_type;
  Function      function;

public:
  BinopCodegen() noexcept  = delete;
  ~BinopCodegen() noexcept = default;
  BinopCodegen(Type::Pointer ret, Function fun) noexcept
      : return_type(ret),
        function(fun) {}
  BinopCodegen(const BinopCodegen &other) noexcept = default;
  BinopCodegen(BinopCodegen &&other) noexcept      = default;
  auto operator=(const BinopCodegen &other) noexcept
      -> BinopCodegen                                           & = default;
  auto operator=(BinopCodegen &&other) noexcept -> BinopCodegen & = default;

  [[nodiscard]] auto ReturnType() const -> Type::Pointer { return return_type; }
  [[nodiscard]] auto operator()(llvm::Value     *left,
                                llvm::Value     *right,
                                CompilationUnit &env) const -> llvm::Value * {
    return function(left, right, env);
  }
};

class BinopOverloadSet {
public:
  using Key       = std::pair<Type::Pointer, Type::Pointer>;
  using Value     = BinopCodegen;
  using Overloads = std::vector<std::pair<Key, Value>>;

  // Proxy class so users don't have to
  // use the Map::Element correctly
  class Overload {
  private:
    Overloads::iterator literal;

  public:
    Overload(Overloads::iterator element) noexcept
        : literal(element) {}
    auto ArgumentTypes() noexcept -> Key { return literal->first; }
    auto ReturnType() noexcept -> Type::Pointer {
      return literal->second.ReturnType();
    }
    auto operator()(llvm::Value     *left,
                    llvm::Value     *right,
                    CompilationUnit &env) const noexcept -> llvm::Value * {
      return literal->second(left, right, env);
    }
  };

private:
  Overloads overloads;

public:
  BinopOverloadSet() noexcept                              = default;
  ~BinopOverloadSet() noexcept                             = default;
  BinopOverloadSet(const BinopOverloadSet &other) noexcept = delete;
  BinopOverloadSet(BinopOverloadSet &&other) noexcept      = default;
  auto operator=(const BinopOverloadSet &other) noexcept
      -> BinopOverloadSet & = delete;
  auto operator=(BinopOverloadSet &&other) noexcept
      -> BinopOverloadSet & = default;

  [[nodiscard]] auto Empty() const noexcept -> bool {
    return overloads.empty();
  }

  auto Register(Type::Pointer          left_type,
                Type::Pointer          right_type,
                Type::Pointer          return_type,
                BinopCodegen::Function generator) -> Overload {
    auto found = Lookup(left_type, right_type);
    if (found) {
      return found.value();
    }

    overloads.emplace_back(std::make_pair(left_type, right_type),
                           BinopCodegen(return_type, generator));
    return std::prev(overloads.end());
  }

  auto Lookup(Type::Pointer left_type, Type::Pointer right_type)
      -> std::optional<Overload> {
    auto cursor = overloads.begin();
    auto end    = overloads.end();
    while (cursor != end) {
      if (Equals(left_type, cursor->first.first) &&
          Equals(right_type, cursor->first.second)) {
        return cursor;
      }
      cursor++;
    }
    return {};
  }
};

/**
 * @brief Represents the table of all known binops
 */
class BinopTable {
public:
  using Key   = Token;
  using Value = std::unique_ptr<BinopOverloadSet>;
  using Table = Map<Key, Value>;

  class Binop {
    BinopTable::Table::Element element;

  public:
    Binop(BinopTable::Table::Element element) noexcept
        : element(element) {}
    [[nodiscard]] auto Empty() const noexcept -> bool {
      return element.Value()->Empty();
    }
    auto Register(Type::Pointer          left_type,
                  Type::Pointer          right_type,
                  Type::Pointer          return_type,
                  BinopCodegen::Function generator)
        -> BinopOverloadSet::Overload {
      return element.Value()->Register(left_type,
                                       right_type,
                                       return_type,
                                       generator);
    }

    auto Lookup(Type::Pointer left_type, Type::Pointer right_type)
        -> std::optional<BinopOverloadSet::Overload> {
      return element.Value()->Lookup(left_type, right_type);
    }
  };

private:
  Table table;

public:
  BinopTable() noexcept                                            = default;
  ~BinopTable() noexcept                                           = default;
  BinopTable(const BinopTable &other) noexcept                     = delete;
  BinopTable(BinopTable &&other) noexcept                          = default;
  auto operator=(const BinopTable &other) noexcept -> BinopTable & = delete;
  auto operator=(BinopTable &&other) noexcept -> BinopTable      & = default;

  // #REASON: we cannot use 'operator' due to it's
  // status as a c++ keyword, so 'op' will have to do.
  // (i dislike 'opr', 'ope', 'oprtr', etc... 'op' just feels clean)
  // NOLINTNEXTLINE(readability-identifier-length)
  auto Register(Token                  op,
                Type::Pointer          left_type,
                Type::Pointer          right_type,
                Type::Pointer          return_type,
                BinopCodegen::Function generator) -> Binop {
    auto found = Lookup(op);
    if (found) {
      found->Register(left_type, right_type, return_type, generator);
      return {found.value()};
    }

    Binop binop = table.Register(op, std::make_unique<BinopOverloadSet>());
    binop.Register(left_type, right_type, return_type, generator);
    return binop;
  }

  // NOLINTNEXTLINE(readability-identifier-length)
  auto Lookup(Token op) -> std::optional<Binop> { return table.Lookup(op); }
};
} // namespace pink
