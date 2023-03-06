#pragma once
#include "type/TypeInterface.h"

#include "llvm/IR/Type.h"

namespace pink {
class Environment;
[[nodiscard]] auto ToLLVM(Type::Pointer type, Environment &env) noexcept
    -> llvm::Type *;
} // namespace pink
