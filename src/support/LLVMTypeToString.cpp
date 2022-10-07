#include "support/LLVMTypeToString.h"

#include "llvm/Support/raw_ostream.h"

namespace pink {
auto LLVMTypeToString(const llvm::Type *type) -> std::string {
  std::string buffer;
  llvm::raw_string_ostream stream(buffer);
  type->print(stream);
  return buffer;
}
} // namespace pink
