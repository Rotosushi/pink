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
 * @file Variable.h
 * @brief Header for class Variable
 * @version 0.1
 */
#pragma once
#include <string>

#include "ast/Ast.h"
#include "aux/StringInterner.h"

namespace pink {
/**
 * @brief Represents a Variable expression
 */
class Variable : public Ast {
private:
  InternedString symbol;

public:
  Variable(const Location &location, InternedString symbol) noexcept
      : Ast(Ast::Kind::Variable, location),
        symbol(symbol) {}
  ~Variable() noexcept override                                = default;
  Variable(const Variable &other) noexcept                     = delete;
  Variable(Variable &&other) noexcept                          = default;
  auto operator=(const Variable &other) noexcept -> Variable & = delete;
  auto operator=(Variable &&other) noexcept -> Variable      & = default;

  auto GetSymbol() noexcept -> InternedString { return symbol; }
  auto GetSymbol() const noexcept -> InternedString { return symbol; }

  static auto classof(const Ast *ast) noexcept -> bool {
    return Ast::Kind::Variable == ast->GetKind();
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
