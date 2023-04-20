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
 * @file Block.h
 * @brief Header for class Block
 * @version 0.1
 */
#pragma once
#include <vector>

#include "ast/Ast.h"

namespace pink {
/**
 * @brief Represents a new scope of expressions
 */
class Block : public Ast {
public:
  using Expressions    = std::vector<Ast::Pointer>;
  using iterator       = Expressions::iterator;
  using const_iterator = Expressions::const_iterator;

private:
  Expressions expressions;

public:
  Block(const Location &location) noexcept
      : Ast(Ast::Kind::Block, location) {}
  Block(const Location &location, Expressions expressions) noexcept
      : Ast(Ast::Kind::Block, location),
        expressions(std::move(expressions)) {}
  ~Block() noexcept override                             = default;
  Block(const Block &other) noexcept                     = delete;
  Block(Block &&other) noexcept                          = default;
  auto operator=(const Block &other) noexcept -> Block & = delete;
  auto operator=(Block &&other) noexcept -> Block      & = default;

  auto IsEmpty() const noexcept -> bool { return begin() == end(); }

  auto GetExpressions() noexcept -> Expressions & { return expressions; }
  auto GetExpressions() const noexcept -> const Expressions & {
    return expressions;
  }

  [[nodiscard]] auto begin() noexcept -> iterator {
    return expressions.begin();
  }
  [[nodiscard]] auto begin() const noexcept -> const_iterator {
    return expressions.begin();
  }
  [[nodiscard]] auto cbegin() const noexcept -> const_iterator {
    return expressions.cbegin();
  }
  [[nodiscard]] auto end() noexcept -> iterator { return expressions.end(); }
  [[nodiscard]] auto end() const noexcept -> const_iterator {
    return expressions.end();
  }
  [[nodiscard]] auto cend() const noexcept -> const_iterator {
    return expressions.cend();
  }

  static auto classof(const Ast *ast) noexcept -> bool {
    return Ast::Kind::Block == ast->GetKind();
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
