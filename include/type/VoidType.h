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
 * @file VoidType.h
 * @brief Header for class VoidType
 * @version 0.1
 */
#pragma once

#include "type/Type.h"

namespace pink {
/**
 * @brief Represents the Type of Void
 */
class VoidType : public Type {
public:
  using Pointer = VoidType const *;

  VoidType(TypeInterner *context, Annotations annotations) noexcept
      : Type(Type::Kind::Void, context, annotations) {}
  ~VoidType() noexcept override                                = default;
  VoidType(const VoidType &other) noexcept                     = default;
  VoidType(VoidType &&other) noexcept                          = default;
  auto operator=(const VoidType &other) noexcept -> VoidType & = default;
  auto operator=(VoidType &&other) noexcept -> VoidType      & = default;

  static auto classof(const Type *type) noexcept -> bool {
    return Type::Kind::Void == type->GetKind();
  }

  auto ToLLVM(CompilationUnit &unit) const noexcept -> llvm::Type * override;

  auto Equals(Type::Pointer right) const noexcept -> bool override {
    return llvm::dyn_cast<const VoidType>(right) != nullptr;
  }

  auto StrictEquals(Type::Pointer right) const noexcept -> bool override {
    if (GetAnnotations() != right->GetAnnotations()) {
      return false;
    }
    return Equals(right);
  }

  void Print(std::ostream &stream) const noexcept override { stream << "Void"; }
};
} // namespace pink
