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
 * @file Integer.h
 * @brief Header for class Integer
 * @version 0.1
 */
#pragma once
#include <string>

#include "ast/Ast.h"

namespace pink {
/**
 * @brief Represents an integer value within an expression
 *
 * can hold any integer from -2^64 to 2^(64) - 1
 *
 * because we choose to have Integer's size be equivalent to the word size of
 * the machine. it's an 'i64' in llvm IR.
 *
 * \todo The size of Integer is not selected dynamically based upon the target
 * machine
 *
 */
class Integer : public Ast {
public:
  using Value = std::size_t;

private:
  /**
   * @brief holds the integer being represented.
   */
  Value value;

public:
  Integer(const Location &location, Value value) noexcept
      : Ast(Ast::Kind::Integer, location),
        value(value) {}
  ~Integer() noexcept override                               = default;
  Integer(const Integer &other) noexcept                     = delete;
  Integer(Integer &&other) noexcept                          = default;
  auto operator=(const Integer &other) noexcept -> Integer & = delete;
  auto operator=(Integer &&other) noexcept -> Integer      & = default;

  auto GetValue() const noexcept -> Value { return value; }

  static auto classof(const Ast *ast) -> bool {
    return Ast::Kind::Integer == ast->GetKind();
  }

  void Accept(AstVisitor *visitor) noexcept override { visitor->Visit(this); }
  void Accept(ConstAstVisitor *visitor) const noexcept override {
    visitor->Visit(this);
  }
};
} // namespace pink
