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
 * @file Boolean.h
 * @brief Header for class Boolean
 * @version 0.1
 *
 */
#pragma once
#include <string>

#include "ast/Ast.h"

namespace pink {
/**
 * @brief [Boolean](#Boolean) is an [Ast](#Ast) node which represents a boolean
 * value
 *
 *
 * @headerfile Boolean.h "ast/Boolean.h"
 */
class Boolean : public Ast {
private:
  bool value;

public:
  Boolean(const Location &location, bool value) noexcept
      : Ast(Ast::Kind::Boolean, location),
        value(value) {}
  ~Boolean() noexcept override                               = default;
  Boolean(const Boolean &other) noexcept                     = delete;
  Boolean(Boolean &&other) noexcept                          = default;
  auto operator=(const Boolean &other) noexcept -> Boolean & = delete;
  auto operator=(Boolean &&other) noexcept -> Boolean      & = default;

  auto GetValue() const noexcept -> bool { return value; }

  static auto classof(const Ast *ast) noexcept -> bool {
    return Ast::Kind::Boolean == ast->GetKind();
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
