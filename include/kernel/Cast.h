/**
 * @file Cast.h
 * @brief Header for Cast
 * @version 0.1
 */
#pragma once

#include "aux/Environment.h"
#include "aux/Outcome.h"

#include "llvm/IR/Type.h"
#include "llvm/IR/Value.h"

namespace pink {
/**
 * @brief Emit the correct bitcast/zext/sext/trunc instruction to convert a
 * value from it's current type, to the target type.
 *
 * \note the only cast implemented currently is Bool -> Int
 *
 * @param value the value to cast
 * @param target_type the target type to cast to
 * @param env the environment of this compilation unit
 * @return Outcome<llvm::Value*, Error> if true the cast value, if false the
 * Error.
 */
Outcome<llvm::Value *, Error> Cast(llvm::Value *value, llvm::Type *target_type,
                                   const Environment &env);
} // namespace pink
