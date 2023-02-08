#pragma once
#include "aux/Environment.h"

#include "type/SliceType.h"

#include "llvm/IR/Value.h"

namespace pink {
/**
 * @brief Performs a bounds checked subscript of the given Slice into an array
 *
 * \note Emits a RuntimeError if the index is out of bounds
 *
 * @param slice_type the layout type of the slice itself
 * @param element_type the element type of the array the slice points into
 * @param slice the pointer to the slice itself
 * @param index the index of the element to retrieve from the array
 * @param env the environment of this compilation unit
 * @return llvm::Value* the element at the index within the array
 */
auto SliceSubscript(llvm::StructType *slice_type,
                    llvm::Type       *element_type,
                    llvm::Value      *slice,
                    llvm::Value      *index,
                    Environment      &env) -> llvm::Value *;
} // namespace pink