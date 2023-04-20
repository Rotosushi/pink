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
#include "type/TupleType.h"

#include "aux/Environment.h"

namespace pink {
auto TupleType::ToLLVM(CompilationUnit &unit) const noexcept -> llvm::Type * {
  std::vector<llvm::Type *> llvm_element_types{};
  llvm_element_types.reserve(elements.size());

  for (const auto *element : elements) {
    llvm_element_types.emplace_back(element->ToLLVM(unit));
  }
  auto *llvm_tuple_type = unit.LLVMStructType(llvm_element_types);
  SetCachedLLVMType(llvm_tuple_type);
  return llvm_tuple_type;
}

auto TupleType::Equals(Type::Pointer right) const noexcept -> bool {
  const auto *other = llvm::dyn_cast<const TupleType>(right);
  if (other == nullptr) {
    return false;
  }

  if (elements.size() != other->elements.size()) {
    return false;
  }

  auto cursor       = elements.begin();
  auto other_cursor = other->elements.begin();
  while (cursor != elements.end()) {
    if (!(*cursor)->Equals(*other_cursor)) {
      return false;
    }

    cursor++;
    other_cursor++;
  }

  return true;
}

void TupleType::Print(std::ostream &stream) const noexcept {
  stream << "(";

  std::size_t index  = 0;
  std::size_t length = elements.size();
  for (const auto *type : elements) {
    type->Print(stream);

    if (index < (length - 1)) {
      stream << ", ";
    }
    index++;
  }

  stream << ")";
}
} // namespace pink
