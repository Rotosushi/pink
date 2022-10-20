#pragma once
#include "aux/Environment.h"

#include "llvm/IR/Value.h"

namespace pink {
void RuntimeError(const std::string &error_description, llvm::Value *exit_code,
                  const Environment &env);
} // namespace pink