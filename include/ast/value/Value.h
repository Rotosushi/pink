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
  enum struct Annotation : unsigned { Temporary, Runtime, SIZE };

private:
  static constexpr auto    bitset_size = Annotation::SIZE;
  std::bitset<bitset_size> set;

public:
  /*
    every literal is a temporary, comptime value.
    (Boolean, Integer, Nil).
  */
  Value(Kind     kind,
        Location location,
        bool     temporary = true,
        bool     runtime   = false) noexcept
      : Ast(kind, location),
        set() {
    set[ToUnderlying(Annotation::Temporary)] = temporary;
    set[ToUnderlying(Annotation::Runtime)]   = runtime;
  }
  ~Value() noexcept override = default;

  void Accept(AstVisitor *visitor) noexcept override            = 0;
  void Accept(ConstAstVisitor *visitor) const noexcept override = 0;
};
} // namespace pink
