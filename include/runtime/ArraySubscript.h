#pragma once

#include "aux/Environment.h"

#include "llvm/IR/Value.h"

namespace pink {
/**
 * @brief emit a bounds checked array subscript '[]' operation
 *
 * \note Emits a RuntimeError if the index is out of bounds
 *
 * @param array_type the type of the array to subscript
 * @param element_type the element type of the array
 * @param array the pointer to the array itself
 * @param index the index to subscript to.
 * @param env the environment of this compilation unit
 * @return llvm::Value* the value held at the index within the array
 */
auto ArraySubscript(llvm::StructType *array_type,
                    llvm::Type       *element_type,
                    llvm::Value      *array,
                    llvm::Value      *index,
                    Environment      &env) -> llvm::Value *;
} // namespace pink
