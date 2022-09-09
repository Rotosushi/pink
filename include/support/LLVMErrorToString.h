#pragma once

#include "llvm/Support/Error.h"


namespace pink {
  std::string LLVMErrorToString(llvm::Error& error);
}
