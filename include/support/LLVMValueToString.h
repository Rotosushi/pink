#pragma once
#include <string>

#include "llvm/IR/Value.h"

namespace pink {
  std::string LLVMValueToString(llvm::Value* value);
}
