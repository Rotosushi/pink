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
#include "llvm/IR/Value.h" // llvm::Value

#include "type/Type.h"                       // pink::Type
#include "type/action/PolymorphicEquality.h" // pink::PolymorphicEquality
#include "type/action/Substitution.h"        // pink::Substitution
#include "type/action/ToString.h"            // pink::ToString

#include "aux/Error.h"   // pink::Error
#include "aux/Map.h"     // pink::Map<K, V>
#include "aux/Outcome.h" // pink::Outcome

#include "front/Token.h" // pink::Token

namespace pink {
class Environment;

class UnopCodegen {
public:
  /**
   * @brief pointer to a function which can be used
   * to generate an llvm IR implementation of a given unop
   */
  using Function = llvm::Value *(*)(llvm::Value *term, Environment &env);

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

  [[nodiscard]] auto operator()(llvm::Value *value,
                                Environment &env) const noexcept
      -> llvm::Value * {
    return function(value, env);
  }
};

class UnopOverloadSet {
public:
  using Key       = Type::Pointer;
  using Value     = UnopCodegen;
  using Overloads = Map<Key, Value>;

  class Overload {
  private:
    Overloads::Element literal;

  public:
    Overload(Overloads::Element element) noexcept
        : literal{std::move(element)} {}
    [[nodiscard]] auto ArgumentType() const noexcept -> Key {
      return literal.Key();
    }
    [[nodiscard]] auto ReturnType() const noexcept -> Type::Pointer {
      return literal.Value().GetReturnType();
    }
    [[nodiscard]] auto operator()(llvm::Value *value,
                                  Environment &env) const noexcept
        -> llvm::Value * {
      return literal.Value()(value, env);
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
    return overloads.Size() == 0;
  }

  auto Register(Type::Pointer         argument_type,
                Type::Pointer         return_type,
                UnopCodegen::Function generator) -> Overload {
    return overloads.Register(argument_type, {return_type, generator});
  }

  auto Lookup(Type::Pointer argument_type) -> std::optional<Overload> {
    return overloads.Lookup(argument_type);
  }
};

/**
 * @brief Represents the set of all known unary operators
 *
 * \todo refactor UnopTable and BinopTable to take a Token
 * as a key, (UnopTable.h, BinopTable.h, Environment.h).
 * Add every defined operator to the list of lexed tokens,
 * (Token.h, Lexer.re). Add Parser::IsOperator(Token token) -> bool,
 * which the Parser::InfixParser can use for parsing binop
 * expressions and the Parser::ParseBasic routine can have the list
 * of defined unops fallthrough to the unop basic case.
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
