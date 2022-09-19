/**
 * @file LLVMValueToString.h
 * @brief Header for LLVMValueToString
 * @version 0.1
 */
#pragma once
#include <string>

#include "llvm/IR/Value.h"

namespace pink {
  /**
   * @brief Print the given llvm::Value to a string
   * 
   * @param value the value to print
   * @return std::string the string which was printed
   */
  std::string LLVMValueToString(llvm::Value* value);
}
