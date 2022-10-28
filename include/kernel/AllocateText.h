#pragma once
#include <string>

#include "aux/Environment.h"

#include "llvm/IR/Value.h"

namespace pink {
/**
 * @brief Allocate a new array containing characters at global scope.
 *
 * \note this is actually an array, so it holds it's length at runtime
 *
 * @param name the name of the global to allocate
 * @param text the text to store within the array
 * @param env the environment of this compilation unit
 * @return llvm::GlobalVariable* the newly allocated text.
 */
auto AllocateGlobalText(const std::string &name, const std::string &text,
                        const Environment &env) -> llvm::GlobalVariable *;

} // namespace pink