/**
 * @file EmitFile.h
 * @brief Header for Emitting files
 * @version 0.1
 */
#pragma once

#include "aux/Environment.h"

namespace pink {
/**
 * @brief Emits any Assembly, LLVM IR, or Object files requested
 * by this compilation environment.
 *
 * @param out the output stream to write any error messages to.
 * @param env the compilation environment
 * @return int either EXIT_SUCCESS or EXIT_FAILURE
 */
auto EmitFiles(std::ostream &out, const Environment &env) -> int;
} // namespace pink