#pragma once

#include "aux/Environment.h"
#include "aux/Error.h"
#include "aux/Outcome.h"

#include "llvm/IR/Value.h"

namespace pink {

auto AllocateVariable(const std::string &name, llvm::Type *type,
                      const Environment &env) -> llvm::Value *;

auto AllocateGlobal(const std::string &name, llvm::Type *type,
                    const Environment &env) -> llvm::GlobalVariable *;

auto AllocateLocal(const std::string &name, llvm::Type *type,
                   const Environment &env) -> llvm::AllocaInst *;

} // namespace pink