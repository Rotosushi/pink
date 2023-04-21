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
#include "ast/ValueOf.h"

#include "aux/Environment.h"

namespace pink {
/*
The type of Value of is T iff the right hand side has type
Pointer<T>

ValueOf can only dereference the pointer, (it's right hand side)
iff that pointer is backed by some memory.
the pointer may be const or mutable, temporary or not.
because pointers only 'exist' at runtime, so does the type
need to be runtime.
*/
auto ValueOf::Typecheck(CompilationUnit &unit) const noexcept
    -> Outcome<Type::Pointer, Error> {
  unit.WithinDereferencePtr(true);
  auto right_outcome = right->Typecheck(unit);
  if (!right_outcome) {
    return right_outcome;
  }
  unit.WithinDereferencePtr(false);
  auto right_type = right_outcome.GetFirst();

  if (const auto *pointer_type = llvm::dyn_cast<PointerType>(right_type);
      pointer_type != nullptr) {
    auto result = pointer_type->GetPointeeType();
    SetCachedType(result);
    return result;
  }

  if (const auto *slice_type = llvm::dyn_cast<SliceType>(right_type);
      slice_type != nullptr) {
    auto result = slice_type->GetPointeeType();
    SetCachedType(result);
    return result;
  }

  std::stringstream errmsg;
  errmsg << "[";
  errmsg << right_type;
  errmsg << "]";
  return Error{Error::Code::CannotDereferenceNonPointer,
               right->GetLocation(),
               std::move(errmsg).str()};
}

auto ValueOf::Codegen(CompilationUnit &unit) const noexcept
    -> Outcome<llvm::Value *, Error> {
  unit.WithinDereferencePtr(true);
  auto right_outcome = right->Codegen(unit);
  if (!right_outcome) {
    unit.WithinDereferencePtr(false);
    return right_outcome;
  }
  unit.WithinDereferencePtr(false);
  auto right_value = right_outcome.GetFirst();

  auto pointee_type = ToLLVM(GetCachedTypeOrAssert(), unit);

  return unit.Load(pointee_type, right_value);
}

void ValueOf::Print(std::ostream &stream) const noexcept {
  stream << "*" << right;
}
} // namespace pink
