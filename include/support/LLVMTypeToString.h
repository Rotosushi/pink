#pragma once 
#include <string>

#include "llvm/IR/Type.h"

namespace pink {
  std::string LLVMTypeToString(llvm::Type* type);
}
