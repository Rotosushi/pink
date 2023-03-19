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

#pragma once

#include "type/Type.h"

#include "aux/StringInterner.h"

namespace pink {
class TypeVariable : public Type {
public:
  using Pointer = TypeVariable const *;

private:
  InternedString identifier;

public:
  TypeVariable(TypeInterner *context, InternedString identifier) noexcept
      : Type(Type::Kind::Variable, context),
        identifier(identifier) {}

  static auto classof(Type const *type) noexcept -> bool {
    return type->GetKind() == Type::Kind::Variable;
  }

  [[nodiscard]] auto Identifier() const noexcept -> InternedString {
    return identifier;
  }

  void Accept(TypeVisitor *visitor) noexcept override { visitor->Visit(this); }
  void Accept(ConstTypeVisitor *visitor) const noexcept override {
    visitor->Visit(this);
  }
};
} // namespace pink
