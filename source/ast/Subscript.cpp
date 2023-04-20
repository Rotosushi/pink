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
#include "ast/Subscript.h"

#include "aux/Environment.h"

namespace pink {
/*
The type of a subscript operation is the element_type of the
subscriptable pointer being accessed if and only if the left
expression has slice or array type, and the right expression
has Integer type.

the lhs of a subscript can be a literal or in memory, it can
be constant or mutable, and it can be temporary or not.
the rhs of a subscript can be a literal or in memory, it can
be constant or mutable, and it can be temporary or not.

the result type is dependent upon the element type of the array.
*/
auto Subscript::Typecheck(CompilationUnit &unit) const noexcept
    -> Outcome<Type::Pointer, Error> {
  auto right_outcome = right->Typecheck(unit);
  if (!right_outcome) {
    return right_outcome;
  }
  auto right_type = right_outcome.GetFirst();

  // #TODO iff the index is known at compile type do the range check
  if (llvm::dyn_cast<IntegerType>(right_type) == nullptr) {
    std::stringstream errmsg;
    errmsg << "Cannot use type [";
    errmsg << right_type;
    errmsg << "] as an index.";
    return Error(Error::Code::SubscriptRightIsNotAnIndex,
                 right->GetLocation(),
                 std::move(errmsg).str());
  }

  auto left_outcome = left->Typecheck(unit);
  if (!left_outcome) {
    return left_outcome;
  }
  auto left_type = left_outcome.GetFirst();

  auto element_outcome = [&]() -> Outcome<Type::Pointer, Error> {
    if (const auto *array_type = llvm::dyn_cast<ArrayType>(left_type);
        array_type != nullptr) {
      return array_type->GetElementType();
    }

    if (const auto *slice_type = llvm::dyn_cast<SliceType>(left_type);
        slice_type != nullptr) {
      return slice_type->GetPointeeType();
    }

    std::stringstream errmsg;
    errmsg << "Cannot subscript type [";
    errmsg << left_type;
    errmsg << "]";
    return Error(Error::Code::SubscriptLeftIsNotSubscriptable,
                 left->GetLocation(),
                 std::move(errmsg).str());
  }();
  if (!element_outcome) {
    return element_outcome;
  }
  const auto *element_type = element_outcome.GetFirst();
  SetCachedType(element_type);
  return element_type;
}

auto Subscript::Codegen(CompilationUnit &unit) const noexcept
    -> Outcome<llvm::Value *, Error> {}

void Subscript::Print(std::ostream &stream) const noexcept {
  stream << left << "[" << right << "]";
}
} // namespace pink
