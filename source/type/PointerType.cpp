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
#include "type/PointerType.h"

#include "aux/Environment.h"

namespace pink {
auto PointerType::ToLLVM(CompilationUnit &unit) const noexcept -> llvm::Type * {
  auto *llvm_pointer_type = unit.LLVMPointerType();
  SetCachedLLVMType(llvm_pointer_type);
  return llvm_pointer_type;
}
} // namespace pink