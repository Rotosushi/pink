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

  we really want to subsume the usages of OnTHeLHSOfAssignment
  WithinAddressOf, WithinDereferencePtr, and WithinBindExpression.
  And since these flags are associated with the Ast itself
  it would be cleaner and clearer to communicate the information
  that needs to be communicated by those flags by way of these
  flags, due to the weirdness of a boolean being manipulated by
  hand within a given tree walking function.

  the first piece of the puzzle is to notice that
  the real purpose of the above flags is to control the emission
  of loads and stores during codegeneration.
  we know a few facts about loads.
  1) we can only load values which are stored in memory
  2) we can only load values which are <= sizeof a single register.
      (llvm::Type::isSingleValueType() checks this.)

  we know a few facts about stores.
  1) we can only store values to memory
  2) we can only store values which are <= sizeof a single register.

  the second piece of this puzzle is what values exist during
  compilation?
  1) literals
    literals are llvm::Constant*s. they do not live in memory or registers, they
    exist as compile time constants. thus they cannot be loaded and can be
  stored 2) values
    -) values are either a register (a) or a memory address in a register (b).
    a) cannot be loaded, can be stored
    b) can be loaded, can be stored

  the third piece of the puzzle is value semantics.
   1) when we say "a := 100;" we mean that a is a memory loaction
    of type Integer which is holding the value 100.
    (allocate a; store 100)

   2) when we say "b := a;" we mean that b is a memory location
    of type Integer which is holding the exact value held in
    the memory location a. 
    (allocate b; store (load a))

   3) when we say "c := a + b;" we mean that c is a memory location
    of type Integer which is holding the sum of the values at memory
    locations a and b. 
    (allocate c; store (load a + load b))
    ((this argument holds for any Integer binop))

   4) when we say "a = c" we mean that the memory location referenced
    by a is assigned the value of the memory location referenced by c.
    (store (load c))

   5) when we say "f(a)" we mean that the value held in the memory location
    referenced by a is passed to f, not a itself
    (allocate parameter, store (load a))

   6) when we say "(a, ...)" we mean that the value held in the memory location
    referenced by a is stored within the tuple itself, not a itself.
    (allocate tuple, store (load a))

   7) when we say "[a, ...]" we mean that the value held in the memory location 
    referenced by a is stored within the array itself, not a itself.
    (allocate array, store (load a))
   
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
