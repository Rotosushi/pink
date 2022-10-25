#pragma once
#include "aux/Environment.h"

#include "type/SliceType.h"

#include "llvm/IR/Value.h"

namespace pink {
auto SliceSubscript(llvm::StructType *slice_type, llvm::Type *element_type,
                    llvm::Value *slice, llvm::Value *index,
                    const Environment &env) -> llvm::Value *;
} // namespace pink