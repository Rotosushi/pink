#pragma once
#include "aux/Environment.h"

#include "type/SliceType.h"

#include "llvm/IR/Value.h"

namespace pink {
auto SliceSubscript(llvm::Type *slice_element_type, llvm::Value *slice,
                    llvm::Value *index, const Environment &env)
    -> llvm::Value *;
} // namespace pink