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
 * @file ArrayType.h
 * @brief Header for class ArrayType
 * @version 0.1
 */
#pragma once
#include "type/Type.h"

namespace pink {
/**
 * @brief Represents the Type of an Array
 */
class ArrayType : public Type {
public:
  using Pointer = ArrayType const *;

private:
  std::size_t   size;
  Type::Pointer element_type;

public:
  ArrayType(TypeInterner *context,
            std::size_t   size,
            Type::Pointer element_type) noexcept
      : Type(Type::Kind::Array, context),
        size(size),
        element_type(element_type) {
    assert(element_type != nullptr);
  }
  ~ArrayType() noexcept override                                 = default;
  ArrayType(const ArrayType &other) noexcept                     = default;
  ArrayType(ArrayType &&other) noexcept                          = default;
  auto operator=(const ArrayType &other) noexcept -> ArrayType & = default;
  auto operator=(ArrayType &&other) noexcept -> ArrayType      & = default;

  [[nodiscard]] auto GetSize() const -> std::size_t { return size; }
  [[nodiscard]] auto GetElementType() const -> Type::Pointer {
    return element_type;
  }

  static auto classof(const Type::Pointer type) noexcept -> bool {
    return Type::Kind::Array == type->GetKind();
  }

  void Print(std::ostream &stream) const noexcept override {
    stream << "[";
    element_type->Print(stream);
    stream << "; ";
    stream << std::to_string(size);
    stream << "]";
  }

  void Accept(TypeVisitor *visitor) noexcept override { visitor->Visit(this); }
  void Accept(ConstTypeVisitor *visitor) const noexcept override {
    visitor->Visit(this);
  }
};
} // namespace pink
