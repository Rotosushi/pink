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
 * @file PointerType.h
 * @brief Header for class PointerType
 * @version 0.1
 */
#pragma once

#include "type/Type.h"

namespace pink {
/**
 * @brief Represents the Type of a Pointer to a single value.
 *
 * 1/23/2023:
 * pointers are integer values, so they can be assigned and
 * passed as parameters, but in the interest of memory safety
 * pointers only support indirection operation. that is unop [*]
 * a pointer can be constructed from any variable with
 * address of, that is unop [&].
 *
 * instead if you want pointer arithmetic you do that via slices,
 * which hold the addressable range they support. then the runtime
 * can check pointer math, and so can the compiler (if the values
 * are literals)
 *
 * a pointer to a slice may be coerced to a slice safely. and
 * any element within the range of a slice must have a valid address.
 *
 * however you may not convert a pointer into a slice.
 *
 * (#NOTE: some computer architechtures use memory mapped I/O
 * for their hardware devices, thus for device drivers it is
 * useful to construct a pointer to an arbitrary memory location.
 * this is something to be considered. if this valid use case
 * was not present I would be on board with saying that pointers
 * can only be constructed via the address of operator. as in there
 * would not be syntax to wite down a pointer literal with.)
 *
 *
 */
class PointerType : public Type {
public:
  using Pointer = PointerType const *;

private:
  Type::Pointer pointee_type;

public:
  PointerType(TypeInterner *context, Type::Pointer pointee_type) noexcept
      : Type(Type::Kind::Pointer, context),
        pointee_type(pointee_type) {
    assert(pointee_type != nullptr);
  }
  ~PointerType() noexcept override                                   = default;
  PointerType(const PointerType &other) noexcept                     = default;
  PointerType(PointerType &&other) noexcept                          = default;
  auto operator=(const PointerType &other) noexcept -> PointerType & = default;
  auto operator=(PointerType &&other) noexcept -> PointerType      & = default;

  static auto classof(const Type::Pointer type) noexcept -> bool {
    return Type::Kind::Pointer == type->GetKind();
  }

  [[nodiscard]] auto GetPointeeType() noexcept -> Type::Pointer {
    return pointee_type;
  }
  [[nodiscard]] auto GetPointeeType() const noexcept -> Type::Pointer {
    return pointee_type;
  }

  void Accept(TypeVisitor *visitor) noexcept override { visitor->Visit(this); }
  void Accept(ConstTypeVisitor *visitor) const noexcept override {
    visitor->Visit(this);
  }
};
} // namespace pink
