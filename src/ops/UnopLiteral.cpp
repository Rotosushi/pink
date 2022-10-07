
#include "ops/UnopLiteral.h"

namespace pink {

UnopLiteral::UnopLiteral(Type *arg_t, Type *ret_t, UnopCodegenFn fn_p) {
  overloads.insert(
      std::make_pair(arg_t, std::make_unique<UnopCodegen>(ret_t, fn_p)));
}

auto UnopLiteral::Register(Type *arg_t, Type *ret_t, UnopCodegenFn fn_p)
    -> std::pair<Type *, UnopCodegen *> {
  auto pair = overloads.insert(
      std::make_pair(arg_t, std::make_unique<UnopCodegen>(ret_t, fn_p)));
  return std::make_pair(pair.first->first, pair.first->second.get());
}

void UnopLiteral::Unregister(Type *arg_t) {
  auto iter = overloads.find(arg_t);

  if (iter != overloads.end()) {
    // UnopLiteral manages the memory of the UnopCodegen instances
    // behind the scenes of the consumer code via std::shared_ptrs
    overloads.erase(iter);
  }
}

auto UnopLiteral::Lookup(Type *arg_t)
    -> llvm::Optional<std::pair<Type *, UnopCodegen *>> {
  auto iter = overloads.find(arg_t);

  if (iter == overloads.end()) {
    return {};
  }

  return {std::make_pair(iter->first, iter->second.get())};
}
} // namespace pink
