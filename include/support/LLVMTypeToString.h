/**
 * @file LLVMTypeToString.h
 * @brief Header for LLVMTypeToString
 * @version 0.1
 */
#pragma once
#include <string>

#include "llvm/IR/Type.h"

namespace pink {
/**
 * @brief Print the given llvm::Type to a string
 *
 * @param type the type to print
 * @return std::string the string that was printed
 */
auto LLVMTypeToString(const llvm::Type *type) -> std::string;
} // namespace pink
