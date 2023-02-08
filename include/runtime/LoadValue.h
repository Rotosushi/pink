#pragma once
#include "aux/Environment.h"

#include "llvm/IR/Value.h"

namespace pink {
/**
 * @brief emit a load instruction if needed given the type.
 *
 * @param type the type to load
 * @param variable the pointer to load from
 * @param env the environment of this compilation unit
 * @return llvm::Value* the loaded variable
 */
auto LoadValue(llvm::Type *type, llvm::Value *variable, Environment &env)
    -> llvm::Value *;
} // namespace pink