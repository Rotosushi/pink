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

namespace pink {
/**
 * @brief Represents the Type of Boolean
 */
class BooleanType : public Type {
public:
  using Pointer = BooleanType const *;

  BooleanType(TypeInterner *context) noexcept
      : Type(Type::Kind::Boolean, context) {}
  ~BooleanType() noexcept override                                   = default;
  BooleanType(const BooleanType &other) noexcept                     = default;
  BooleanType(BooleanType &&other) noexcept                          = default;
  auto operator=(const BooleanType &other) noexcept -> BooleanType & = default;
  auto operator=(BooleanType &&other) noexcept -> BooleanType      & = default;

  static auto classof(const Type *type) noexcept -> bool {
    return Type::Kind::Boolean == type->GetKind();
  }

  void Print(std::ostream &stream) const noexcept override {
    stream << "Boolean";
  }

  void Accept(TypeVisitor *visitor) noexcept override { visitor->Visit(this); }
  void Accept(ConstTypeVisitor *visitor) const noexcept override {
    visitor->Visit(this);
  }
};
} // namespace pink
