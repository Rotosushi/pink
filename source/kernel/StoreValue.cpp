#include "kernel/StoreValue.h"
#include "kernel/StoreAggregate.h"

namespace pink {
void StoreValue(llvm::Type *type, llvm::Value *destination, llvm::Value *source,
                const Environment &env) {
  assert(type != nullptr);
  assert(destination != nullptr);
  assert(source != nullptr);
  // #RULE we can only store single value types.
  if (type->isSingleValueType()) {
    env.instruction_builder->CreateStore(source, destination);
  } else {
    StoreAggregate(type, destination, source, env);
  }
}
} // namespace pink
