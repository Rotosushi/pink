#include "support/LLVMErrorToString.h"

#include "llvm/Support/raw_ostream.h"

namespace pink {
auto LLVMErrorToString(const llvm::Error &error) -> std::string {
  std::string buffer;
  llvm::raw_string_ostream stream(buffer);
  stream << error;
  return buffer;
}
} // namespace pink
