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

#include "ast/visitor/AstVisitor.h"

namespace pink {
class Type;

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
  Kind kind;
  Location location;
  mutable Type *cached_type;

public:
  Ast(Kind kind, Location location) noexcept : kind(kind), location(location) {}
  virtual ~Ast() noexcept = default;
  Ast(const Ast &other) noexcept = delete;
  Ast(Ast &&other) noexcept = default;
  auto operator=(const Ast &other) noexcept -> Ast & = delete;
  auto operator=(Ast &&other) noexcept -> Ast & = default;

  [[nodiscard]] auto GetKind() noexcept -> Kind { return kind; }
  [[nodiscard]] auto GetKind() const noexcept -> Kind { return kind; }
  [[nodiscard]] auto GetLocation() noexcept -> Location & { return location; }
  [[nodiscard]] auto GetLocation() const noexcept -> const Location & {
    return location;
  }

  [[nodiscard]] auto GetCachedType() noexcept -> std::optional<const Type *> {
    if (cached_type == nullptr) {
      return {};
    }
    return {cached_type};
  }

  [[nodiscard]] auto GetCachedType() const noexcept
      -> std::optional<const Type *> {
    if (cached_type == nullptr) {
      return {};
    }
    return {cached_type};
  }

  void SetCachedType(Type *type) const noexcept { cached_type = type; }

  virtual void Accept(AstVisitor *visitor) noexcept = 0;
  virtual void Accept(ConstAstVisitor *visitor) const noexcept = 0;

  /**
   * @brief Computes the [Type](#Type) of this [term](#Ast)
   *
   * This function runs the, as far as I know, standard simple typing algorithm,
   * eagerly evaluated, with no implicit casting. There are currently
   * no syntactic forms which allow for user types. (other than functions)
   *
   * \todo implement some form of user defined types
   *
   *
   * @param env The Environment to typecheck against, an
   * [Environment](#Environment) set up as if by [NewGlobalEnv](#NewGlobalEnv)
   * is suitable
   * @return Outcome<Type*, Error> if the type checking algorithm could assign
   * this term a [type](#Type) then the Outcome holds that type, otherwise the
   * Outcome holds the Error that the type checking algorithm constructed.
   */
  //[[nodiscard]] auto Typecheck(const Environment &env) const
  //    -> Outcome<Type *, Error>;

  /**
   * @brief Computes the LLVM Intermediate Representation ([IR]) corresponding
   * to this Ast.
   *
   * [IR]: https://llvm.org/docs/LangRef.html "IR"
   *
   * Internally this function uses the [instruction_builder] to construct the
   * [IR]. and does so with respect to the [symbol_table], [binop_table],
   * [unop_table], and [llvm_module], held within the environment parameter.
   * (this is not intended as a complete list)
   *
   * [symbol_table]: @ref SymbolTable "symbol table"
   * [binop_table]: @ref BinopTable "binop table"
   * [unop_table]: @ref UnopTable "unop table"
   * [instruction_builder]: https://llvm.org/doxygen/classllvm_1_1IRBuilder.html
   * "instruction builder" [llvm_module]:
   * https://llvm.org/doxygen/classllvm_1_1Module.html "llvm_module"
   *
   * @param env The Environment to generate code against, an
   * [Environment](#Environment) set up as if by [NewGlobalEnv](#NewGlobalEnv)
   * is suitable
   * @return Outcome<llvm::Value*, Error> if the code generator could assign
   * this term a [value] then the Outcome holds this value, otherwise the
   * Outcome holds the [Error] the code generator constructed.
   */
  //[[nodiscard]] virtual auto Codegen(const Environment &env) const
  //    -> Outcome<llvm::Value *, Error> = 0;
};
} // namespace pink
