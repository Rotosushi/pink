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
 * @file While.h
 * @brief Header for class While
 * @version 0.1
 */
#pragma once

#include "ast/Ast.h"

namespace pink {
/**
 * @brief Represents an instance of a While loop
 */
class While : public Ast {
private:
  Ast::Pointer test;
  Ast::Pointer body;

public:
  While(Location location, Ast::Pointer test, Ast::Pointer body) noexcept
      : Ast(Ast::Kind::While, location),
        test(std::move(test)),
        body(std::move(body)) {}
  ~While() noexcept override                             = default;
  While(const While &other) noexcept                     = delete;
  While(While &&other) noexcept                          = default;
  auto operator=(const While &other) noexcept -> While & = delete;
  auto operator=(While &&other) noexcept -> While      & = default;

  static auto
  Create(Location location, Ast::Pointer test, Ast::Pointer right) noexcept {
    return std::make_unique<While>(location, std::move(test), std::move(right));
  }

  auto GetTest() noexcept -> Ast::Pointer & { return test; }
  auto GetTest() const noexcept -> const Ast::Pointer & { return test; }
  auto GetBody() noexcept -> Ast::Pointer & { return body; }
  auto GetBody() const noexcept -> const Ast::Pointer & { return body; }

  static auto classof(const Ast *ast) noexcept -> bool {
    return Ast::Kind::While == ast->GetKind();
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
