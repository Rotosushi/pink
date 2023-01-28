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

#include "type/Type.h"

#include "ast/visitor/AstVisitor.h"

namespace pink {

/*
  Would it be faster to store the entire Ast into a vector?
  to 'flatten' the tree, then would Typecheck and Compile
  run faster?

  a few problems:
    - Pointers become invalidated if the vector
    ever resizes, unfortunate when we are building up a tree
    from source text and will be appending who knows how many
    nodes before the tree is complete.

    if you ever want to modify the tree now you have to copy
    the entire thing to a new vector per modification.
    and if you store a vector of unique_ptr, then how is
    that any different than having the nodes themselves store
    unique_ptr's?

    - any node of the tree representing another sequence of terms
    doesn't have a knowable size, and so would need to store a
    set of pointers.

    -

*/

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
    Application,
    Assignment,
    Bind,
    Binop,
    Block,
    Boolean,
    Array,
    Conditional,
    Dot,
    Function,
    Integer,
    Nil,
    Subscript,
    Tuple,
    Unop,
    Variable,
    While,
  };

private:
  Kind          kind;
  Location      location;
  mutable Type *cached_type;

public:
  Ast(Kind kind, Location location) noexcept : kind(kind), location(location) {}
  virtual ~Ast() noexcept                            = default;
  Ast(const Ast &other) noexcept                     = delete;
  Ast(Ast &&other) noexcept                          = default;
  auto operator=(const Ast &other) noexcept -> Ast & = delete;
  auto operator=(Ast &&other) noexcept -> Ast      & = default;

  [[nodiscard]] auto GetKind() noexcept -> Kind { return kind; }
  [[nodiscard]] auto GetKind() const noexcept -> Kind { return kind; }
  [[nodiscard]] auto GetLocation() noexcept -> Location & { return location; }
  [[nodiscard]] auto GetLocation() const noexcept -> const Location & {
    return location;
  }

  [[nodiscard]] auto GetCachedType() noexcept -> std::optional<Type::Pointer> {
    if (cached_type == nullptr) {
      return {};
    }
    return {cached_type};
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

  void SetCachedType(Type *type) const noexcept { cached_type = type; }

  virtual void Accept(AstVisitor *visitor) noexcept            = 0;
  virtual void Accept(ConstAstVisitor *visitor) const noexcept = 0;
};
} // namespace pink
