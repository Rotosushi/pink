#include "support/LLVMValueToString.h"

#include "llvm/Support/raw_ostream.h"

namespace pink {
auto LLVMValueToString(const llvm::Value *value) -> std::string {
  std::string buffer;
  llvm::raw_string_ostream stream(buffer);
  value->print(stream);
  return buffer;
}
} // namespace pink
