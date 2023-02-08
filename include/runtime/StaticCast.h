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
 * \note the only cast implemented currently is Boolean -> Integer
 *
 * \note This function is not in general safe to use.
 * Nor is it the most robust solution. I would prefer
 * a solution which allows the user to define new casts,
 * for these casts to be explicit or implicit, and
 * for casts to be safe if the language defines them.
 *
 * since we have yet to define any dynamic type system,
 * all types are static and knowable at compile time.
 * so there is no real reason to implement any dynamic_cast,
 * yet. once we have tagged unions, it would be cool to have
 * a simple way to match over the active member, and have some
 * dynamic cast like statement to convert the union into the
 * member type for a single conversion situation.
 * and along with that use, if we add structures we could also
 * add inheritance, though maybe not virtual methods.
 * and I think function overloading is awesome, along
 * with function templates.
 *
 *
 *
 * @param value the value to cast
 * @param target_type the target type to cast to
 * @param env the environment of this compilation unit
 * @return Outcome<llvm::Value*, Error> if true the cast value, if false the
 * Error.
 */
auto StaticCast(llvm::Value *value, llvm::Type *target_type, Environment &env)
    -> Outcome<llvm::Value *, Error>;
} // namespace pink
