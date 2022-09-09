#include "support/LLVMTypeToString.h"

#include "llvm/Support/raw_ostream.h"

namespace pink {
  std::string LLVMTypeToString(llvm::Type* type)
  {
    std::string buffer;
    llvm::raw_string_ostream stream(buffer);
    type->print(stream);
    return buffer;
  }
}
