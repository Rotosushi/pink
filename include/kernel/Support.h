#pragma once
#include <string>

#include "llvm/IR/Value.h"
#include "llvm/IR/Type.h"

namespace pink {
  
  std::string ToString(llvm::Value* value);
  std::string ToString(llvm::Type* type);

}



