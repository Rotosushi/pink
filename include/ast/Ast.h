/**
 * @file Ast.h
 * @brief Header file for Ast, the base class of any class which represents a
 * node within a given Abstract syntax tree.
 * @version 0.1
 *
 * @copyright Copyright (c) 2022
 *
 */

// #QUESTION:
// Should we refactor Getype, Codegen, and ToString to be visitors?
// we have a few extra arguments compared to the basic visitor pattern
// but this is in order to pass data to and from the visitor so it can actually
// work. if we pass in a AstVisitor& to the accept function, then we
// are forced to simply trust the programmer to provide the correct
// visitor for the Outcome<T, U>& result they pass in, and the
// result must be an argument because we cannot overload a function
// on it's return type, and we cannot define a single accept method for
// any given Visitor implementation because we cannot template a virtual method!
// the only way to handle this is to reimplement the Outcome class
// to use inheritance to handle the difference between a codegen outcome or
// a typecheck outcome or so on. however, then we are trusting that the
// outcome that we receive as an argument is the correct derived Outcome,
// which means that is still not as type safe as the current way of implementing
// the 'visitors'. (meaning we have to add code to check that things are okay to
// access) as far as i can tell right now, this is about how we have to
// implement the visitor pattern from the perspective of the Ast. because of: 1)
// we need some way of communicating the resulting error or type of a typecheck,
//    the resulting error or value of a codegen, OR the resulting string of a
//    ToString, and it needs to work such that the
//    Typechecking/Codegeneration/ToString routines can use the results from
//    calling the same routine on the child nodes of the current ast node. this
//    is the basic way the algorithm builds up the type of expressions.
// 2) we need to do the work with respect to the Environment data structure,
//    because that is where the SymbolTable, BinopTable, UnopTable,
//    llvm::IRBuilder, and other important data structures are grouped, such
//    that codegeneration and typechecking can be done with respect to those
//    data structures. this is fundamentally because we must record symbols as
//    they are defined such that we can access their values later to implement
//    expressions referencing those symbols.
// void Accept(const TypeChecker &tc, Outcome<Type*, Error>& result,
// std::shared_ptr<Environment> env) = 0; void Accept(const CodeGenerator &cg,
// Outcome<llvm::Value*, Error>& result, std::shared_ptr<Environment> env) = 0;
// void Accept(const StringGenerator &sg, std::string& result,
// std::shared_ptr<Environment> env) = 0;

// the visitor pattern increases code size and mental complexity, because
// we add a new class definition to the program per kind of walk we want
// to do. I guess if we had numerous kinds of walks then each Ast node's
// file would have to contain snippets of several algorithms in the same source
// file, so we gain the ease of declaring the entire algorithm in a single
// source file, implementing the visitor for each ast node kind. so in that
// sense the algorithm is easier to see all at once. however each class kind
// takes similar steps within it's peice of each algorithm. so that becomes
// harder to see if the whole algorithm is within the visitor file.
// also, the whole typechecking algorithm is about as big (maybe even bigger
// now?) as the parser, so the size of the typechecking file would be big
// (around 1000 - 1500 lines), and get bigger as we added new node kinds. so we
// aren't really reducing the number of massive files in the project, so much as
// we are removing a large amount of implementation from the individual files of
// the Ast. the Ast code as it stands now gets bigger each time we add a new
// kind of walk to the tree. just as the typecheckvisitor file gets larger as we
// add more kinds of node.
// it seems to me that debugging and profiling code would be emitted alonside
// the code that is already emitted, and they don't need to be their own
// walks of the tree. just have flags set that control the emission.
// so what kind of walks do we need to add to the AST now or in the future?
// -) ah, each optimization that we would want to add would need to walk the
// tree, and it would be easier to build and maintain separate classes for
// each optimization kind, rather than increase the code size of the Ast,
// per optimization supported.
// this might be a consideration if the code we emit to llvm cannot be
// optimized by llvm well, due to the way we are emitting the llvm
// representation. so we could employ optimization walks to
// modify such code. so adding the visitor pattern can be safely put off
// until we have a need to add another kind of walk.
// -) any code sanatizer or formatting function could make use of the visitor
// pattern as well.

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
   *  is currently instanciated when inspecting a variable of type (Ast*)
   *
   */
  enum class Kind {
    Block,
    Variable,
    VarRef,
    Bind,
    Assignment,
    Dot,
    Function,
    Application,
    Conditional,
    While,

    Tuple,
    Array,

    Binop,
    Unop,

    Nil,
    Bool,
    Int,
  };

private:
  Kind kind;
  Location loc;
  Type *type;

  /** @property kind
   * @brief holds the [kind](#Kind) of this Ast node
   */
  /** @property loc
   * @brief holds the textual [location](#Location) of this Ast node
   */
  /** @property type
   * @brief initially nullptr; however after a call to Getype type caches the
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
  [[nodiscard]] virtual auto GetypeV(const Environment &env) const
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
   * \note: the type is only filled in an Ast after Getype has been
   * called on this Ast. That is, immediately after construction of
   * an Ast, GetType will return nullptr,
   *
   * @return const Type* the type of this ast, or nullptr
   */
  [[nodiscard]] auto GetType() const -> Type *;

  /**
   * @brief Computes the canonical string representation of this Ast
   *
   * by canonical I simply mean that if the [parser](#Parser) were to read in
   * the string returned by ToString it would construct the exact same Ast as
   * generated the string. That is, Parser::Parse() and Ast::ToString() are
   * inverse operations. (modulo unnecessary textual formatting)
   *
   * \todo ToString ends up doing a lot of intermediate allocations
   * and concatenations as it builds up it's result, this seems like
   * a good place to consider optimizing.
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
   * This function runs a fairly standard simple typing algorithm,
   * eagerly evaluated, with no implicit casting. There are currently
   * no syntactic forms which allow for user types. (other than functions)
   *
   * internally this function retreives all instances of a [Type](#Type)
   * via the [TypeInterner](#TypeInterner) so all type comparison can be
   * accomplished via pointer comparison.
   *
   * \todo implement some form of user defined types
   *
   * \note Getype itself only modifies the Ast in that if a type
   * was computed, then that type is cached within the Ast node,
   * so subsequet calls to Getype are faster. This is because we
   * also need the Type of any given term available within Codegeneration.
   * and redoing all that work is slow.
   *
   * @param env The Environment to typecheck against, an
   * [Environment](#Environment) set up as if by [NewGlobalEnv](#NewGlobalEnv)
   * is suitable
   * @return Outcome<Type*, Error> if the type checking algorithm could assign
   * this term a [type](#Type) then the Outcome holds that type, otherwise the
   * Outcome holds the Error that the type checking algorithm constructed.
   */
  [[nodiscard]] auto Getype(const Environment &env) -> Outcome<Type *, Error>;

  /**
   * @brief Computes the LLVM Intermediate Representation ([IR]) corresponding
   * to this Ast.
   *
   * [IR]: https://llvm.org/docs/LangRef.html "IR"
   *
   * Internally this function uses the [instruction_builder] to construct the
   * [IR]. and does so with respect to the [symbol_table], [binop_table],
   * [unop_table], and [module], held within the environment parameter. (this is
   * not intended as a complete list)
   *
   * [symbol_table]: @ref SymbolTable "symbol table"
   * [binop_table]: @ref BinopTable "binop table"
   * [unop_table]: @ref UnopTable "unop table"
   * [instruction_builder]: https://llvm.org/doxygen/classllvm_1_1IRBuilder.html
   * "instruction builder" [module]:
   * https://llvm.org/doxygen/classllvm_1_1Module.html "module"
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
