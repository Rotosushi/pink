// Copyright (C) 2023 Cade Weinberg
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
class AddressOf : public Ast {
public:
private:
  Ast::Pointer right;

public:
  AddressOf(const Location &location, Ast::Pointer right) noexcept
      : Ast(Ast::Kind::AddressOf, location),
        right(std::move(right)) {}
  ~AddressOf() noexcept override                                 = default;
  AddressOf(const AddressOf &other) noexcept                     = delete;
  AddressOf(AddressOf &&other) noexcept                          = default;
  auto operator=(const AddressOf &other) noexcept -> AddressOf & = delete;
  auto operator=(AddressOf &&other) noexcept -> AddressOf      & = default;

  [[nodiscard]] auto GetRight() noexcept -> Ast::Pointer & { return right; }
  [[nodiscard]] auto GetRight() const noexcept -> const Ast::Pointer & {
    return right;
  }

  static auto classof(const Ast *ast) -> bool {
    return ast->GetKind() == Ast::Kind::AddressOf;
  }

  void Accept(AstVisitor *visitor) noexcept override { visitor->Visit(this); }
  void Accept(ConstAstVisitor *visitor) const noexcept override {
    visitor->Visit(this);
  }
};
} // namespace pink
