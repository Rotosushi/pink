#pragma once

#include "aux/Environment.h"

#include "llvm/IR/Value.h"

namespace pink {
void StoreValue(llvm::Type *type, llvm::Value *destination, llvm::Value *source,
                const Environment &env);
}
