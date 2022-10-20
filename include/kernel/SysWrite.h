#pragma once

#include "aux/Environment.h"

#include "llvm/IR/Value.h"

namespace pink {
auto SysWrite(llvm::Value *file_descriptor, llvm::Value *slice_ptr,
              const Environment &env) -> llvm::Value *;
}