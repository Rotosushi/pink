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
class CharacterType : public Type {
public:
  using Pointer = CharacterType const *;

  CharacterType(TypeInterner *context) noexcept
      : Type(Type::Kind::Character, context) {}
  ~CharacterType() noexcept override                 = default;
  CharacterType(const CharacterType &other) noexcept = default;
  CharacterType(CharacterType &&other) noexcept      = default;
  auto operator=(const CharacterType &other) noexcept
      -> CharacterType                                            & = default;
  auto operator=(CharacterType &&other) noexcept -> CharacterType & = default;

  static auto classof(const Type *type) noexcept -> bool {
    return Type::Kind::Character == type->GetKind();
  }

  void Accept(TypeVisitor *visitor) noexcept override { visitor->Visit(this); }
  void Accept(ConstTypeVisitor *visitor) const noexcept override {
    visitor->Visit(this);
  }
};
} // namespace pink