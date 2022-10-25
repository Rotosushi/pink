#pragma once

#include "aux/Environment.h"

#include "llvm/IR/Value.h"

namespace pink {
auto SysWriteSlice(llvm::Value *file_descriptor, llvm::StructType *slice_type,
                   llvm::Value *slice_ptr, const Environment &env)
    -> llvm::Value *;

auto SysWriteText(llvm::Value *file_descriptor, llvm::StructType *text_type,
                  llvm::Value *text_ptr, const Environment &env)
    -> llvm::Value *;

auto SysWrite(llvm::Value *file_descriptor, llvm::Value *size,
              llvm::Value *buffer, const Environment &env) -> llvm::Value *;
} // namespace pink