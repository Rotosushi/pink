/**
 * @file UnopTable.h
 * @brief Header for class UnopTable
 * @version 0.1
 */
#pragma once
#include "llvm/IR/Value.h" // llvm::Value

#include "type/Type.h" // pink::Type

#include "aux/Map.h"            // pink::Map<K, V>
#include "aux/StringInterner.h" // pink::InternedString

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

/**
 * @brief Represents a single unary operator
 */
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
        : literal(element) {}
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
  ~UnopOverloadSet() noexcept                            = default;
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

class PolyUnopOverloadSet : UnopOverloadSet {
public:
  
}

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
  using Key   = InternedString;
  using Value = UnopOverloadSet;
  using Table = Map<Key, Value>;

  class Unop {
  private:
    Table::Element element;

  public:
    Unop(Table::Element element) noexcept
        : element(element) {}
    [[nodiscard]] auto Empty() const noexcept -> bool {
      return element.Value().Empty();
    }
    [[nodiscard]] auto Operator() const noexcept -> InternedString {
      return element.Key();
    }
    auto Register(Type::Pointer         argument_type,
                  Type::Pointer         return_type,
                  UnopCodegen::Function generator)
        -> UnopOverloadSet::Overload {
      return element.Value().Register(argument_type, return_type, generator);
    }
    [[nodiscard]] auto Lookup(Type::Pointer argument_type)
        -> std::optional<UnopOverloadSet::Overload> {
      return element.Value().Lookup(argument_type);
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

  auto Register(InternedString        op,
                Type::Pointer         argument_type,
                Type::Pointer         return_type,
                UnopCodegen::Function generator) -> Unop {
    auto found = Lookup(op);
    if (found) {
      found->Register(argument_type, return_type, generator);
      return found.value();
    }

    Unop unop = table.Register(op, {});
    unop.Register(argument_type, return_type, generator);
    return unop;
  }

  auto Lookup(InternedString op) -> std::optional<Unop> {
    return table.Lookup(op);
  }
};
} // namespace pink
