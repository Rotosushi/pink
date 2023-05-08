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
 * @file Tuple.h
 * @brief Header for class Tuple
 * @version 0.1
 */
#pragma once
#include <vector>

#include "ast/Ast.h"

namespace pink {
/**
 * @brief Represents a Tuple expression
 *
 *
 *  can also be thought of as an anonymous
 *  struct type with numbered element access instead
 *  of named element access.
 *
 *  the underlying data-type of structs and tuples are
 *  going to be identical, the only real difference is that
 *  the struct type holds a Look Up Table mapping the names to the
 *  element offset.
 *
 *  elements can be either single value sized, or
 *  greater, and as such we must modify a Function
 *  when necessesary to support returning them by value
 *  from a Function or passing them by value to a Function.
 */
class Tuple : public Ast {
public:
  using Elements       = std::vector<Ast::Pointer>;
  using iterator       = Elements::iterator;
  using const_iterator = Elements::const_iterator;

private:
  Elements elements;

public:
  Tuple(Location location, Elements elements) noexcept
      : Ast(Ast::Kind::Tuple, location),
        elements(std::move(elements)) {}
  ~Tuple() noexcept override                             = default;
  Tuple(const Tuple &other) noexcept                     = delete;
  Tuple(Tuple &&other) noexcept                          = default;
  auto operator=(const Tuple &other) noexcept -> Tuple & = delete;
  auto operator=(Tuple &&other) noexcept -> Tuple      & = default;

  static auto Create(Location location, Elements elements) noexcept {
    return std::make_unique<Tuple>(location, std::move(elements));
  }

  auto GetElements() noexcept -> Elements & { return elements; }
  auto GetElements() const noexcept -> const Elements & { return elements; }

  [[nodiscard]] auto begin() noexcept -> iterator { return elements.begin(); }
  [[nodiscard]] auto begin() const noexcept -> const_iterator {
    return elements.begin();
  }
  [[nodiscard]] auto cbegin() const noexcept -> const_iterator {
    return elements.cbegin();
  }
  [[nodiscard]] auto end() noexcept -> iterator { return elements.end(); }
  [[nodiscard]] auto end() const noexcept -> const_iterator {
    return elements.end();
  }
  [[nodiscard]] auto cend() const noexcept -> const_iterator {
    return elements.cend();
  }

  static auto classof(const Ast *ast) noexcept -> bool {
    return Ast::Kind::Tuple == ast->GetKind();
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
