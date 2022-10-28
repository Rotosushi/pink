#pragma once
#include "aux/Environment.h"

#include "llvm/IR/Value.h"

namespace pink {
/**
 * @brief Implements a RuntimeError.
 *
 * \note Prints the given error to STDERR and then exits the process.
 *
 * @param error_description the string to be printed to STDERR
 * @param exit_code the exit_code to be passed to sys_exit
 * @param env the environment of this compilation unit.
 */
void RuntimeError(const std::string &error_description, llvm::Value *exit_code,
                  const Environment &env);
} // namespace pink