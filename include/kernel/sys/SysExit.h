#pragma once

#include "aux/Environment.h"

#include "llvm/IR/Value.h"

namespace pink {
/**
 * @brief implements the llvm assembly to call the
 * sys_exit x86-64 linux system call.
 *
 * @param exit_code the exit code to pass to sys_exit
 * @param env the environment of this compilation unit
 */
void CodegenSysExit(llvm::Value *exit_code, const Environment &env);
} // namespace pink