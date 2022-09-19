/**
 * @file LLVMErrorToString.h
 * @brief Header for LLVMErrorToString
 * @version 0.1
 */
#pragma once

#include "llvm/Support/Error.h"


namespace pink {
  /**
   * @brief Prints the given llvm::Error to a string
   * 
   * @param error the error to print
   * @return std::string the string that was printed
   */
  std::string LLVMErrorToString(llvm::Error& error);
}
