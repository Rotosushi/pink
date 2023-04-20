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
#include "type/ArrayType.h"

#include "aux/Environment.h"

namespace pink {
auto ArrayType::ToLLVM(CompilationUnit &unit) const noexcept -> llvm::Type * {
  auto *llvm_element_type = element_type->ToLLVM(unit);
  auto *llvm_array_type   = unit.LLVMArrayType(llvm_element_type, size);
  SetCachedLLVMType(llvm_array_type);
  return llvm_array_type;
}

auto ArrayType::Equals(Type::Pointer right) const noexcept -> bool {
  const auto *other = llvm::dyn_cast<const ArrayType>(right);
  if (other == nullptr) {
    return false;
  }

  if (size != other->size) {
    return false;
  }

  return element_type->Equals(other->element_type);
}

void ArrayType::Print(std::ostream &stream) const noexcept {
  stream << "[";
  element_type->Print(stream);
  stream << "; ";
  stream << std::to_string(size);
  stream << "]";
}
} // namespace pink
