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
#include "ast/value/Value.h"

namespace pink {
class Boolean : public Value {
public:
private:
  bool value;

public:
  Boolean(Location location,
          bool     value,
          bool     temporary = true,
          bool     comptime  = true) noexcept
      : Value(Ast::Kind::Boolean, location, temporary, comptime),
        value(value) {}
  ~Boolean() noexcept override                               = default;
  Boolean(const Boolean &other) noexcept                     = delete;
  Boolean(Boolean &&other) noexcept                          = default;
  auto operator=(const Boolean &other) noexcept -> Boolean & = delete;
  auto operator=(Boolean &&other) noexcept -> Boolean      & = default;

  [[nodiscard]] auto Value() noexcept -> bool & { return value; }
  [[nodiscard]] auto Value() const noexcept -> const bool & { return value; }

  void Accept(AstVisitor *visitor) noexcept override;
  void Accept(ConstAstVisitor *visitor) const noexcept override;
};
} // namespace pink
