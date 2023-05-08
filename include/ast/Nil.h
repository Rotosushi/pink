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
 * @file Nil.h
 * @brief Header for class Nil
 * @version 0.1
 *
 */
#pragma once
#include <iostream>
#include <string>

#include "ast/Ast.h"

namespace pink {
/**
 * @brief Represents the single valued type 'nil'
 *
 * it's value is equivalent to 'false.'
 * it's useful for many of the same reasons the
 * type 'void' is useful in C/C++ and has marginally
 * more uses than a true void type
 */
class Nil : public Ast {
public:
  Nil(Location location) noexcept
      : Ast(Ast::Kind::Nil, location) {}
  ~Nil() noexcept override                           = default;
  Nil(const Nil &other) noexcept                     = delete;
  Nil(Nil &&other) noexcept                          = default;
  auto operator=(const Nil &other) noexcept -> Nil & = delete;
  auto operator=(Nil &&other) noexcept -> Nil      & = default;

  static auto Create(Location location) noexcept {
    return std::make_unique<Nil>(location);
  }

  static auto classof(const Ast *ast) -> bool {
    return Ast::Kind::Nil == ast->GetKind();
  }

  auto Typecheck(CompilationUnit &unit) const noexcept
      -> Outcome<Type::Pointer> override;
  auto Codegen(CompilationUnit &unit) const noexcept
      -> Outcome<llvm::Value *> override;
  void Print(std::ostream &stream) const noexcept override;

  void Accept(AstVisitor *visitor) noexcept override { visitor->Visit(this); }
  void Accept(ConstAstVisitor *visitor) const noexcept override {
    visitor->Visit(this);
  }
};
} // namespace pink
