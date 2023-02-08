#pragma once

#include "aux/Environment.h"

#include "llvm/IR/Value.h"

namespace pink {
/**
 * @brief Abstracts the difference between storing a singleValueType and an
 * Aggregate type into a single function call.
 *
 * @param type the type to be stored
 * @param destination the destination to store into
 * @param source the source to store (must be already loaded)
 * @param env the environment of this compilation unit
 */
void StoreValue(llvm::Type  *type,
                llvm::Value *destination,
                llvm::Value *source,
                Environment &env);
} // namespace pink
