#include "support/LLVMErrorToString.h"

#include "llvm/Support/raw_ostream.h"

namespace pink {
  std::string LLVMErrorToString(llvm::Error& error)
  {
    std::string buffer;
    llvm::raw_string_ostream stream(buffer);
    stream << error;
    return buffer;
  }
}
