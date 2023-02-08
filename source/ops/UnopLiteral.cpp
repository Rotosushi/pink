
#include "ops/UnopLiteral.h"

namespace pink {

auto UnopLiteral::Register(Type::Pointer arg_t, Type::Pointer ret_t,
                           UnopCodegenFn fn_p)
    -> std::pair<Type::Pointer, UnopCodegen *> {
  auto pair = overloads.insert(
      std::make_pair(arg_t, std::make_unique<UnopCodegen>(ret_t, fn_p)));
  return std::make_pair(pair.first->first, pair.first->second.get());
}

void UnopLiteral::Unregister(Type::Pointer arg_t) {
  auto iter = overloads.find(arg_t);

  if (iter != overloads.end()) {
    // UnopLiteral manages the memory of the UnopCodegen instances
    // behind the scenes of the consumer code via std::shared_ptrs
    overloads.erase(iter);
  }
}

auto UnopLiteral::Lookup(Type::Pointer arg_t)
    -> std::optional<std::pair<Type::Pointer, UnopCodegen *>> {
  auto iter = overloads.find(arg_t);

  if (iter == overloads.end()) {
    return {};
  }

  return {std::make_pair(iter->first, iter->second.get())};
}
} // namespace pink
