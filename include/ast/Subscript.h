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

#pragma once
#include "ast/Ast.h"

namespace pink {
/**
 * @brief Implements array subscription
 */
class Subscript : public Ast {
private:
  Ast::Pointer left;
  Ast::Pointer right;

public:
  Subscript(const Location &location,
            Ast::Pointer    left,
            Ast::Pointer    right) noexcept
      : Ast(Ast::Kind::Subscript, location),
        left(std::move(left)),
        right(std::move(right)) {}
  ~Subscript() noexcept override                                 = default;
  Subscript(const Subscript &other) noexcept                     = delete;
  Subscript(Subscript &&other) noexcept                          = default;
  auto operator=(const Subscript &other) noexcept -> Subscript & = delete;
  auto operator=(Subscript &&other) noexcept -> Subscript      & = default;

  auto GetLeft() noexcept -> Ast::Pointer & { return left; }
  auto GetLeft() const noexcept -> const Ast::Pointer & { return left; }
  auto GetRight() noexcept -> Ast::Pointer & { return right; }
  auto GetRight() const noexcept -> const Ast::Pointer & { return right; }

  static auto classof(const Ast *ast) noexcept -> bool {
    return Ast::Kind::Subscript == ast->GetKind();
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