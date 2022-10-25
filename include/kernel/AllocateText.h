#pragma once
#include <string>

#include "aux/Environment.h"

#include "llvm/IR/Value.h"

namespace pink {
auto AllocateGlobalText(const std::string &name, const std::string &text,
                        const Environment &env) -> llvm::GlobalVariable *;

} // namespace pink