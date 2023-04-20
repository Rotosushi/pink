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
 * @file Binop.h
 * @brief Header for class Binop
 * @version 0.1
 */
#pragma once
#include <string>

#include "ast/Ast.h"

#include "ops/BinopTable.h"

#include "front/Token.h"

#include "type/ArrayType.h"
#include "type/IntType.h"
#include "type/SliceType.h"

#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"

namespace pink {
/**
 * @brief Represents single binary operator expression
 */
class Binop : public Ast {
private:
  Token        op;
  Ast::Pointer left;
  Ast::Pointer right;

public:
  Binop(const Location &location,
        Token           opr,
        Ast::Pointer    left,
        Ast::Pointer    right) noexcept
      : Ast(Ast::Kind::Binop, location),
        op(opr),
        left(std::move(left)),
        right(std::move(right)) {}
  ~Binop() noexcept override                             = default;
  Binop(const Binop &other) noexcept                     = delete;
  Binop(Binop &&other) noexcept                          = default;
  auto operator=(const Binop &other) noexcept -> Binop & = delete;
  auto operator=(Binop &&other) noexcept -> Binop      & = default;

  auto GetOp() noexcept -> Token { return op; }
  auto GetOp() const noexcept -> Token { return op; }
  auto GetLeft() noexcept -> Ast::Pointer & { return left; }
  auto GetLeft() const noexcept -> const Ast::Pointer & { return left; }
  auto GetRight() noexcept -> Ast::Pointer & { return right; }
  auto GetRight() const noexcept -> const Ast::Pointer & { return right; }

  static auto classof(const Ast *ast) noexcept -> bool {
    return Ast::Kind::Binop == ast->GetKind();
  }

  auto Typecheck(CompilationUnit &unit) const noexcept
      -> Outcome<Type::Pointer, Error> override;
  auto Codegen(CompilationUnit &unit) const noexcept
      -> Outcome<llvm::Value *, Error> override;
  void Print(std::ostream &stream) const noexcept override;

  void Accept(AstVisitor *visitor) noexcept override { visitor->Visit(this); }
  void Accept(ConstAstVisitor *visitor) const noexcept override {
    visitor->Visit(this);
  }
};
} // namespace pink
