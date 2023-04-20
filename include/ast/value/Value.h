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
#include <bitset> // std::bitset

#include "ast/Ast.h"

#include "support/ToUnderlying.h"

namespace pink {
class Value : public Ast {
public:
  enum struct Annotation : unsigned { Temporary, Comptime, SIZE };

private:
  static constexpr auto    bitset_size = ToUnderlying(Annotation::SIZE);
  std::bitset<bitset_size> set;

public:
  /*
    every literal is a temporary, comptime value.
    (Boolean, Integer, Nil).
  */
  Value(Ast::Kind kind,
        Location  location,
        bool      temporary = true,
        bool      comptime  = true) noexcept
      : Ast(kind, location),
        set() {
    set[ToUnderlying(Annotation::Temporary)] = temporary;
    set[ToUnderlying(Annotation::Comptime)]  = comptime;
  }
  ~Value() noexcept override                             = default;
  Value(const Value &other) noexcept                     = delete;
  Value(Value &&other) noexcept                          = default;
  auto operator=(const Value &other) noexcept -> Value & = delete;
  auto operator=(Value &&other) noexcept -> Value      & = default;

  [[nodiscard]] auto IsTemporary() const noexcept -> bool {
    return set[ToUnderlying(Annotation::Temporary)];
  }
  auto IsTemporary(bool state) noexcept -> bool {
    return set[ToUnderlying(Annotation::Temporary)] = state;
  }
  [[nodiscard]] auto IsComptime() const noexcept -> bool {
    return set[ToUnderlying(Annotation::Comptime)];
  }
  auto IsComptime(bool state) noexcept -> bool {
    return set[ToUnderlying(Annotation::Comptime)] = state;
  }

  void Accept(AstVisitor *visitor) noexcept override            = 0;
  void Accept(ConstAstVisitor *visitor) const noexcept override = 0;
};
} // namespace pink
