#pragma once

#include "aux/Environment.h"

#include "llvm/IR/Value.h"

namespace pink {
auto ArraySubscript(llvm::StructType *array_type, llvm::Type *element_type,
                    llvm::Value *array, llvm::Value *index,
                    const Environment &env) -> llvm::Value *;
}
