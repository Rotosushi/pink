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
 * @brief The type of slices
 *
 *
 * a slice is a pointer composed together with the
 * size of the allocation it points too.
 *
 * An Array can be coerced into a slice safely.
 * the only consideration is the fact that we
 * store an array as the length plus the array,
 * so a slice pointing to the array will be a
 * copy of the length plus a pointer to the array
 * allocation. Thus using a slice in this way incurs
 * a bit of overhead compared to direct array access
 * within a local scope.
 * however passing an array to a function using a slice
 * is very efficient compared to passing the array
 * itself by value.
 *
 * an array in the return slot of a function, as the language
 * is currently defined must have compile time known size.
 * and then it can be allocated in the calling scope and
 * simply written to in the callee scope. And this should
 * work up a chain of calls.
 *
 */
class SliceType : public Type {
public:
  using Pointer = SliceType const *;

private:
  Type::Pointer pointee_type;

public:
  SliceType(TypeInterner *context, Type::Pointer pointee_type) noexcept
      : Type(Type::Kind::Slice, context),
        pointee_type(pointee_type) {
    assert(pointee_type != nullptr);
  }
  ~SliceType() noexcept override                                 = default;
  SliceType(const SliceType &other) noexcept                     = default;
  SliceType(SliceType &&other) noexcept                          = default;
  auto operator=(const SliceType &other) noexcept -> SliceType & = default;
  auto operator=(SliceType &&other) noexcept -> SliceType      & = default;

  static auto classof(const Type *type) noexcept -> bool {
    return Type::Kind::Slice == type->GetKind();
  }

  [[nodiscard]] auto GetPointeeType() const noexcept -> Type::Pointer {
    return pointee_type;
  }

  auto ToLLVM(CompilationUnit &unit) const noexcept -> llvm::Type * override;

  auto Equals(Type::Pointer right) const noexcept -> bool override {
    const auto *other = llvm::dyn_cast<const SliceType>(right);
    if (other == nullptr) {
      return false;
    }

    return pointee_type->Equals(other->pointee_type);
  }

  void Print(std::ostream &stream) const noexcept override {
    stream << "*[]";
    pointee_type->Print(stream);
  }
};
} // namespace pink