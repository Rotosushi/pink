#include "support/LLVMValueToString.h"

#include "llvm/Support/raw_ostream.h"

namespace pink {
  std::string LLVMValueToString(llvm::Value* value)
  { 
    std::string buffer;
    llvm::raw_string_ostream stream(buffer);
    value->print(stream);
    return buffer;
  }
}
