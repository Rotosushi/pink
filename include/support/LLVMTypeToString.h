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
 * @file LLVMTypeToString.h
 * @brief Header for LLVMTypeToString
 * @version 0.1
 */
#pragma once
#include <string>

#include "llvm/IR/Type.h"

#include "llvm/Support/raw_ostream.h"

namespace pink {
/**
 * @brief Print the given llvm::Type to a string
 *
 * @param type the type to print
 * @return std::string the string that was printed
 */
inline auto LLVMTypeToString(const llvm::Type *type) -> std::string {
  std::string buffer;
  llvm::raw_string_ostream stream(buffer);
  type->print(stream);
  return buffer;
}
} // namespace pink
