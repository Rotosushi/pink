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
