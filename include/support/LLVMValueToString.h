/**
 * @file LLVMValueToString.h
 * @brief Header for LLVMValueToString
 * @version 0.1
 */
#pragma once
#include <string>

#include "llvm/IR/Value.h"

#include "llvm/Support/raw_ostream.h"

namespace pink {
/**
 * @brief Print the given llvm::Value to a string
 *
 * @param value the value to print
 * @return std::string the string which was printed
 */
inline auto LLVMValueToString(const llvm::Value *value) -> std::string {
  std::string buffer;
  llvm::raw_string_ostream stream(buffer);
  value->print(stream);
  return buffer;
}
} // namespace pink
