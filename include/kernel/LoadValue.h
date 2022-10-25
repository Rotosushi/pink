#pragma once
#include "aux/Environment.h"

#include "llvm/IR/Value.h"

namespace pink {
auto LoadValue(llvm::Type *type, llvm::Value *variable, const Environment &env)
    -> llvm::Value *;
}