#include "runtime/LoadValue.h"

namespace pink {
auto LoadValue(llvm::Type *type, llvm::Value *value, Environment &env)
    -> llvm::Value * {
  assert(type != nullptr);
  assert(value != nullptr);

  // #RULE we can only load single value types
  if (type->isSingleValueType()) {
    // #RULE assignment needs a pointer on the left to perform assignment
    // #RULE address of converts the single value type to it's pointer
    if (env.internal_flags.OnTheLHSOfAssignment() ||
        env.internal_flags.WithinAddressOf()) {
      return value;
    }
    return env.instruction_builder->CreateLoad(type, value);
  }
  return value;
}
} // namespace pink