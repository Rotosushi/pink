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
#include <ostream>  // std::ostream

#include "llvm/Support/Casting.h" // llvm::isa, llvm::cast, etc

#include "support/ToUnderlying.h" // pink::ToUnderlying

#include "aux/Error.h"    // pink::Error
#include "aux/Location.h" // pink::Location
#include "aux/Outcome.h"  // pink::Outcome

#include "type/Type.h" // pink::Type

#include "ast/visitor/AstVisitor.h"

namespace llvm {
class Value;
}

namespace pink {
/*
  okay, so it seemed weird at the time, however, if we
  add a split in the Ast heirarchy between Expressions
  and Values we can extend the compiler to perform evaluation
  of Asts at compile time.

  I think it would be a cool to design & build around
  sticking to the phrase
  "if it can happen at comptime it does happen at comptime."
*/

/**
 * @brief Ast is the base class of all abstract syntax tree nodes
 *
 * \note Ast is pure virtual
 */
class Ast {
private:
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
    AddressOf,
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
    ValueOf,
    While,
    LastExpression,

    Nil,
    Boolean,
    Integer,
    Array,
    Tuple,
    LastValue,
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

  virtual auto Typecheck(CompilationUnit &unit) const noexcept
      -> Outcome<Type::Pointer> = 0;
  virtual auto Codegen(CompilationUnit &unit) const noexcept
      -> Outcome<llvm::Value *>                           = 0;
  virtual void Print(std::ostream &stream) const noexcept = 0;

  virtual void Accept(AstVisitor *visitor) noexcept            = 0;
  virtual void Accept(ConstAstVisitor *visitor) const noexcept = 0;
};

inline auto Typecheck(const Ast::Pointer &ast, CompilationUnit &unit) noexcept {
  return ast->Typecheck(unit);
}

inline auto operator<<(std::ostream &stream, const Ast::Pointer &ast)
    -> std::ostream & {
  ast->Print(stream);
  return stream;
}
} // namespace pink
