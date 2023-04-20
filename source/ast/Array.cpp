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
#include "ast/Array.h"

#include "aux/Environment.h"

namespace pink {

/* 4/20/2023
The type of an Array is an ArrayType of the
same length and type as the Array if and only if
each element of the Array is the same type as the
first element of the Array.

an array is-a literal iff all it's elements are literals
otherwise we construct an in-memory type.
when we construct an array literal it is constant,
when we construct an array in memory it is mutable,
when we construct an array it is a temporary value.
(it must be bound to something to have it's lifetime extended)
*/
auto Array::Typecheck(CompilationUnit &unit) const noexcept
    -> Outcome<Type::Pointer, Error> {
  std::vector<Type::Pointer> element_types;
  element_types.reserve(elements.size());

  for (auto &element : elements) {
    auto element_outcome = element->Typecheck(unit);
    if (!element_outcome) {
      return element_outcome;
    }
    auto element_type = element_outcome.GetFirst();

    element_types.emplace_back(element_type);

    if (!element_type->Equals(element_types[0])) {
      std::stringstream errmsg;
      errmsg << "Element type [";
      errmsg << element_type;
      errmsg << "] does not match type predicted by first element [";
      errmsg << element_types[0];
      errmsg << "]";
      return Error(Error::Code::ArrayMemberTypeMismatch,
                   element->GetLocation(),
                   std::move(errmsg).str());
    }
  }

  auto array_type = unit.GetArrayType(elements.size(), element_types[0]);
  SetCachedType(array_type);
  return array_type;
}

/*
  we generate an array literal on the stack

  #TODO: emit stack lifetime intrisics for the array
*/
auto Array::Codegen(CompilationUnit &unit) const noexcept
    -> Outcome<llvm::Value *, Error> {
  auto cached_type = GetCachedTypeOrAssert();

  auto *layout_type = llvm::cast<llvm::StructType>(ToLLVM(cached_type, unit));

  auto alloca = unit.CreateAlloca(layout_type, nullptr, "array");

  std::vector<llvm::Value *> actual_elements;
  actual_elements.reserve(elements.size());
  for (auto &element : elements) {
    auto element_outcome = element->Codegen(unit);
    if (!element_outcome) {
      return element_outcome;
    }
    actual_elements.emplace_back(element_outcome.GetFirst());
  }

  unit.StoreArray(layout_type, alloca, actual_elements);
  return alloca;
}

void Array::Print(std::ostream &stream) const noexcept {
  stream << "[";

  std::size_t index  = 0;
  std::size_t length = elements.size();
  for (const auto &element : elements) {
    stream << element;

    if (index < (length - 1)) {
      stream << ", ";
    }
    index++;
  }

  stream << "]";
}
} // namespace pink
