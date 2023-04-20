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
 * @file Unop.h
 * @brief Header for class Unop
 * @version 0.1
 */
#pragma once
#include <string>

#include "ast/Ast.h"
#include "front/Token.h"

namespace pink {
/**
 * @brief Represents a unary operator expression
 *
 * \todo #CPP can be lowered to cpp unops
 */
class Unop : public Ast {
private:
  Token        op;
  Ast::Pointer right;

public:
  Unop(const Location &location, Token opr, Ast::Pointer right) noexcept
      : Ast(Ast::Kind::Unop, location),
        op(opr),
        right(std::move(right)) {}
  ~Unop() noexcept override                            = default;
  Unop(const Unop &other) noexcept                     = delete;
  Unop(Unop &&other) noexcept                          = default;
  auto operator=(const Unop &other) noexcept -> Unop & = delete;
  auto operator=(Unop &&other) noexcept -> Unop      & = default;

  auto GetOp() noexcept -> Token { return op; }
  auto GetOp() const noexcept -> Token { return op; }
  auto GetRight() noexcept -> Ast::Pointer & { return right; }
  auto GetRight() const noexcept -> const Ast::Pointer & { return right; }

  static auto classof(const Ast *ast) noexcept -> bool {
    return Ast::Kind::Unop == ast->GetKind();
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
