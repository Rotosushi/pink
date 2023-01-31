#pragma once
#include "type/Type.h"

#include "llvm/IR/Type.h"

namespace pink {
class Environment;
[[nodiscard]] auto ToLLVM(Type::Pointer type, Environment &env) noexcept
    -> llvm::Type *;
} // namespace pink
