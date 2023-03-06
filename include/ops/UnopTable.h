/**
 * @file UnopTable.h
 * @brief Header for class UnopTable
 * @version 0.1
 */
#pragma once
#include "llvm/IR/Value.h" // llvm::Value

#include "type/TypeInterface.h"       // pink::Type
#include "type/action/Substitution.h" // pink::Substitution

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
  TypeInterface::Pointer return_type;
  Function               function;

public:
  UnopCodegen() noexcept
      : return_type{nullptr},
        function{nullptr} {}
  UnopCodegen(TypeInterface::Pointer return_type, Function function) noexcept
      : return_type{return_type},
        function{function} {}
  ~UnopCodegen() noexcept                                            = default;
  UnopCodegen(const UnopCodegen &other) noexcept                     = default;
  UnopCodegen(UnopCodegen &&other) noexcept                          = default;
  auto operator=(const UnopCodegen &other) noexcept -> UnopCodegen & = default;
  auto operator=(UnopCodegen &&other) noexcept -> UnopCodegen      & = default;

  [[nodiscard]] auto GetReturnType() const noexcept -> TypeInterface::Pointer {
    return return_type;
  }

  [[nodiscard]] auto operator()(llvm::Value *value,
                                Environment &env) const noexcept
      -> llvm::Value * {
    return function(value, env);
  }
};

class BuiltinUnopOverloadSet {
public:
  using Key       = TypeInterface::Pointer;
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
    [[nodiscard]] auto ReturnType() const noexcept -> TypeInterface::Pointer {
      return literal.Value().GetReturnType();
    }
    [[nodiscard]] auto operator()(llvm::Value *value,
                                  Environment &env) const noexcept
        -> llvm::Value * {
      return literal.Value()(value, env);
    }
  };

protected:
  // #REASON: this is protected so the derived classes
  // can implement their specific behavior over this
  // set of data. and this class is the abstract base
  // to make calling code simpler.
  // NOLINTNEXTLINE(cppcoreguidelines-non-private-member-variables-in-classes)
  Overloads overloads;

public:
  BuiltinUnopOverloadSet() noexcept          = default;
  virtual ~BuiltinUnopOverloadSet() noexcept = default;
  BuiltinUnopOverloadSet(const BuiltinUnopOverloadSet &other) noexcept = delete;
  BuiltinUnopOverloadSet(BuiltinUnopOverloadSet &&other) noexcept = default;
  auto operator=(const BuiltinUnopOverloadSet &other) noexcept
      -> BuiltinUnopOverloadSet & = delete;
  auto operator=(BuiltinUnopOverloadSet &&other) noexcept
      -> BuiltinUnopOverloadSet & = default;

  [[nodiscard]] auto Empty() const noexcept -> bool {
    return overloads.Size() == 0;
  }

  virtual auto Register(TypeInterface::Pointer argument_type,
                        TypeInterface::Pointer return_type,
                        UnopCodegen::Function  generator) -> Overload = 0;

  virtual auto Lookup(TypeInterface::Pointer argument_type)
      -> std::optional<Overload> = 0;
};

/**
 * @brief Represents a single unary operator
 */
class ConcreteBuiltinUnopOverloadSet : public BuiltinUnopOverloadSet {
public:
  auto Register(TypeInterface::Pointer argument_type,
                TypeInterface::Pointer return_type,
                UnopCodegen::Function  generator) -> Overload override {
    return overloads.Register(argument_type, {return_type, generator});
  }

  auto Lookup(TypeInterface::Pointer argument_type)
      -> std::optional<Overload> override {
    return overloads.Lookup(argument_type);
  }
};

/*
 * TemplateBuiltinUnopOverloadSet is not built to solve
 * the general problem of a templated unary operator,
 * rather this class is used to add the 'templated'
 * unop builtins '&' and '*'. which are implementable
 * over all possible types given the same UnopCodegen::Function.
 * (due to llvm always returning a pointer to memory allocations.
 *  both '&' and '*' are implemented as no-ops)
 */
class TemplateBuiltinUnopOverloadSet : public BuiltinUnopOverloadSet {
public:
  // to define a template, we need to define some
  // form of type substitution. which can carry us
  // from having Type Variables in the definition
  // of a given type, to the new type with no variables
  // within it.
private:
  UnopCodegen::Function  function;
  TypeInterface::Pointer poly_return_type;
  TypeInterface::Pointer poly_argument_type;
  TypeInterface::Pointer type_variable;

public:
  TemplateBuiltinUnopOverloadSet(TypeInterface::Pointer type_variable,
                                 TypeInterface::Pointer poly_return_type,
                                 TypeInterface::Pointer poly_argument_type,
                                 UnopCodegen::Function  function)
      : function(function),
        poly_return_type(poly_return_type),
        poly_argument_type(poly_argument_type),
        type_variable(type_variable) {}

  auto Register(TypeInterface::Pointer argument_type,
                TypeInterface::Pointer return_type,
                UnopCodegen::Function  generator) -> Overload override {
    return overloads.Register(argument_type, {return_type, generator});
  }

  auto Lookup(TypeInterface::Pointer argument_type)
      -> std::optional<Overload> override {
    auto found = overloads.Lookup(argument_type);
    if (found) {
      return found;
    }

    auto const *mono_return_type =
        Substitution(argument_type, type_variable, poly_return_type);
    auto const *mono_argument_type =
        Substitution(argument_type, type_variable, poly_argument_type);
    return overloads.Register(mono_argument_type, {mono_return_type, function});
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
  using Key   = InternedString;
  using Value = std::unique_ptr<BuiltinUnopOverloadSet>;
  using Table = Map<Key, Value>;

  class Unop {
  private:
    Table::Element element;

  public:
    Unop(Table::Element element) noexcept
        : element(element) {}
    [[nodiscard]] auto Empty() const noexcept -> bool {
      return element.Value()->Empty();
    }
    [[nodiscard]] auto Operator() const noexcept -> InternedString {
      return element.Key();
    }
    auto Register(TypeInterface::Pointer argument_type,
                  TypeInterface::Pointer return_type,
                  UnopCodegen::Function  generator)
        -> BuiltinUnopOverloadSet::Overload {
      return element.Value()->Register(argument_type, return_type, generator);
    }
    [[nodiscard]] auto Lookup(TypeInterface::Pointer argument_type)
        -> std::optional<BuiltinUnopOverloadSet::Overload> {
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
  auto RegisterBuiltin(InternedString         op,
                       TypeInterface::Pointer argument_type,
                       TypeInterface::Pointer return_type,
                       UnopCodegen::Function  generator) -> Unop {
    assert(op != nullptr);
    assert(argument_type != nullptr);
    assert(return_type != nullptr);
    assert(generator != nullptr);
    auto found = Lookup(op);
    if (found) {
      found->Register(argument_type, return_type, generator);
      return found.value();
    }

    Unop unop =
        table.Register(op, std::make_unique<ConcreteBuiltinUnopOverloadSet>());
    unop.Register(argument_type, return_type, generator);
    return unop;
  }

  // NOLINTNEXTLINE(readability-identifier-length)
  auto RegisterTemplateBuiltin(InternedString         op,
                               TypeInterface::Pointer type_variable,
                               TypeInterface::Pointer argument_type,
                               TypeInterface::Pointer return_type,
                               UnopCodegen::Function  function) -> Unop {
    assert(op != nullptr);
    assert(type_variable != nullptr);
    assert(argument_type != nullptr);
    assert(return_type != nullptr);
    assert(function != nullptr);
    auto found = Lookup(op);
    assert(!found); // there can only be a templated op, or a non-templated op.
    // #NOTE: 3/5/2023 this might be a bit too restrictive an assert,
    // technically we are fine in the case that the already defined
    // unop is templated. but checking that requires a dynamic cast.
    // and this is a temporary solution to be subsumed by a full
    // template solution.
    Unop unop = table.Register(
        op,
        std::make_unique<TemplateBuiltinUnopOverloadSet>(type_variable,
                                                         return_type,
                                                         argument_type,
                                                         function));
    return unop;
  }

  // NOLINTNEXTLINE(readability-identifier-length)
  auto Lookup(InternedString op) -> std::optional<Unop> {
    assert(op != nullptr);
    return table.Lookup(op);
  }
};
} // namespace pink
