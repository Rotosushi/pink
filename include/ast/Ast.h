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
#include <iostream>
#include <memory>
#include <string>

#include "llvm/Support/Casting.h"

#include "aux/Error.h"
#include "aux/Location.h"
#include "aux/Outcome.h"

#include "type/Type.h"

#include "llvm/IR/Value.h"

namespace pink {
class Environment;
class AstVisitor;
class ConstAstVisitor;

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

*/

/**
 * @brief Ast is the base class of all abstract syntax tree nodes
 *
 *  Any class written to represent a node within the Ast must inherit from Ast.
 *
 * \note Ast is pure virtual, so there is no way to construct a plain Ast,
 * only an instance of a derived Ast may be constructed.
 */
class Ast {
public:
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
  Location loc;
  mutable Type *type;

  /** @property kind
   * @brief holds the [kind](#Kind) of this Ast node
   */
  /** @property loc
   * @brief holds the textual [location](#Location) of this Ast node
   */
  /** @property type
   * @brief initially nullptr; however after a call to Typecheck it caches the
   * [type](#Type) of this Ast node
   */

  /**
   * @brief The pure virtual method which implements type checking of this Ast
   * expression
   *
   * @param env The Environment to type check against; An
   * [Environment](#Environment) as if constructed by
   * [NewGlobalEnv](#NewGlobalEnv)
   *
   * @return Outcome<Type*, Error> if this Ast could be typed by the type
   * checker, then the Outcome holds that type, otherwise the Outcome holds the
   * Error the type checker ran into during evaluation.
   */
  [[nodiscard]] virtual auto TypecheckV(const Environment &env) const
      -> Outcome<Type *, Error> = 0;

public:
  /**
   * @brief Construct a new Ast object
   *
   * \warning Ast is a virtual base class, and so an instance of an Ast cannot
   * be constructed directly, only an instance of a derived class may be
   * constructed.
   *
   * @param kind the [kind](#Kind) of the ast object being constructed
   * @param location the textual [location](#Location) of the ast object being
   * constructed
   */
  Ast(Kind kind, Location location);

  /**
   * @brief Destroy an Ast object
   */
  virtual ~Ast() = default;

  Ast(const Ast &other) = delete;

  Ast(Ast &&other) = default;

  auto operator=(const Ast &other) -> Ast & = delete;

  auto operator=(Ast &&other) noexcept -> Ast & = default;

  /**
   * @brief Get the Kind
   *
   * @return Kind the [kind](#Kind) of the Ast object
   */
  [[nodiscard]] auto GetKind() const -> const Kind &;

  /**
   * @brief Get the Loc
   *
   * @return Location the syntactic [location](#Location) of the Ast object
   */
  [[nodiscard]] auto GetLoc() const -> const Location &;

  /**
   * @brief Get the Type
   *
   * \note: the type is only filled in an Ast after Typecheck has been
   * called on this Ast. That is, immediately after construction of
   * an Ast, GetType will return nullptr, and only after the Ast passed
   * Typechecking will this return a valid type pointer.
   *
   * @return const Type* the type of this ast, or nullptr
   */
  [[nodiscard]] auto GetType() const -> Type *;

  /**
   * @brief part of the Visitor interface
   *
   * @param visitor the visitor to accept
   */
  // virtual void Accept(AstVisitor *visitor) = 0;

  /**
   * @brief part of the Visitor interface
   *
   * @param visitor the visitor to accept
   */
  virtual void Accept(const ConstAstVisitor *visitor) const = 0;

  /**
   * @brief Computes the canonical string representation of this Ast
   *
   * by canonical I simply mean that if the [parser](#Parser) were to read in
   * the string returned by ToString it would construct the exact same Ast as
   * generated the string. That is, Parser::Parse() and Ast::ToString() are
   * inverse operations. (modulo textual formatting)
   *
   * \todo ToString ends up doing a lot of intermediate allocations
   * and concatenations as it builds up it's result, this seems like
   * a good place to consider optimizing. To accomplish this it might
   * work to buffer all of the strings into a list, and then perform
   * a single concatenation at the end. This might speed up the algorithm
   * overall. as there may be less total allocations, and there will be
   * no repeated copying of the intermediate strings.
   *
   * \todo This function does not currently indent blocks of code
   * according to their nested depth. and I feel that it should.
   *
   * @return std::string the canonical string representing this Ast
   */
  [[nodiscard]] virtual auto ToString() const -> std::string = 0;

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
  [[nodiscard]] auto Typecheck(const Environment &env) const
      -> Outcome<Type *, Error>;

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
  [[nodiscard]] virtual auto Codegen(const Environment &env) const
      -> Outcome<llvm::Value *, Error> = 0;
};
} // namespace pink
