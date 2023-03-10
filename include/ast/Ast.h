/**
 * @file Ast.h
 * @brief Header file for Ast, the base class of any class which represents a
 * node within a given Abstract syntax tree.
 * @version 0.1
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once
#include <memory>   // std::unique_ptr
#include <optional> // std::optional

#include "llvm/Support/Casting.h" // llvm::isa, llvm::cast, etc

#include "aux/Location.h" // pink::Location

#include "type/Type.h" // pink::Type

#include "ast/visitor/AstVisitor.h"

namespace pink {
/*
  #NOTE: 2/19/2023
  -) We can probably make Ast (and Type) Header only.
  -) Ast is getting a modification, We are adding two
      new Derived Interfaces; Expression and Value.
      Expressions are evaluated by the compiler.
      Value is for Mutables and Constants
    -) do we make Conditional and Loop interfaces for
      the different constructs? or is that too many layers
      for no good reason?
*/

/*
  #TODO: what flags would we want to associate with a given
  Ast?
  mutability

*/
class AstFlags {
public:
private:
  enum Flags {
    Mutable,
    SIZE // must be the last enum value
         // no enums may have an assigned value
  };

public:
};

/**
 * @brief Ast is the base class of all abstract syntax tree nodes
 *
 * \note Ast is pure virtual
 */
class Ast {
public:
  using Pointer = std::unique_ptr<Ast>;
  /**
   * @brief Ast::Kind is defined so as to conform to LLVM style [RTTI]
   *
   * [RTTI]: https://llvm.org/docs/HowToSetUpLLVMStyleRTTI.html "RTTI"
   *
   *
   *  Ast::Kind is used as a tag to identify which node kind
   *  is currently instanciated when inspecting a variable of type (Ast*),
   * this can be accomplished by calling Ast::GetKind or by calling
   * classof() on any derived type.
   *
   */
  enum class Kind {
    // Expressions
    Application,
    Assignment,
    Bind,
    Binop,
    Block,
    IfThenElse,
    Dot,
    Function,
    Subscript,
    Unop,
    Variable,
    While,
    // Values
    Nil,
    Boolean,
    Integer,
    Array,
    Tuple,
  };

private:
  Kind                  kind;
  Location              location;
  mutable Type::Pointer cached_type;

public:
  Ast(Kind kind, Location location) noexcept
      : kind{kind},
        location{location},
        cached_type{nullptr} {}
  virtual ~Ast() noexcept                            = default;
  Ast(const Ast &other) noexcept                     = delete;
  Ast(Ast &&other) noexcept                          = default;
  auto operator=(const Ast &other) noexcept -> Ast & = delete;
  auto operator=(Ast &&other) noexcept -> Ast      & = default;

  [[nodiscard]] auto GetKind() const noexcept -> Kind { return kind; }
  [[nodiscard]] auto GetLocation() noexcept -> Location & { return location; }
  [[nodiscard]] auto GetLocation() const noexcept -> const Location & {
    return location;
  }

  [[nodiscard]] auto GetCachedType() const noexcept
      -> std::optional<Type::Pointer> {
    if (cached_type == nullptr) {
      return {};
    }
    return cached_type;
  }

  [[nodiscard]] auto GetCachedTypeOrAssert() const noexcept -> Type::Pointer {
    assert(cached_type != nullptr);
    return cached_type;
  }

  void SetCachedType(Type::Pointer type) const noexcept { cached_type = type; }

  virtual void Accept(AstVisitor *visitor) noexcept            = 0;
  virtual void Accept(ConstAstVisitor *visitor) const noexcept = 0;
};
} // namespace pink
