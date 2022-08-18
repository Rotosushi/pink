#include "kernel/Support.h"

#include "llvm/Support/raw_ostream.h"


namespace pink {
  
  std::string ToString(llvm::Value* value)
  {
    std::string buffer;
    llvm::raw_string_ostream result(buffer);
    value->print(result);
    return result.str();
  }


  std::string ToString(llvm::Type* type)
  {
    std::string buffer;
    llvm::raw_string_ostream result(buffer);
    type->print(result);
    return result.str();
  }


}
