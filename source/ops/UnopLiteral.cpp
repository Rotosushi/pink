
#include "ops/UnopLiteral.h"

namespace pink {

auto UnopLiteral::Register(Type::Pointer arg_t,
                           Type::Pointer ret_t,
                           UnopCodegenFn fn_p) -> UnopCodegen * {
  auto &pair = overloads.emplace_back(arg_t, UnopCodegen{ret_t, fn_p});
  return std::addressof(pair.second);
}

auto UnopLiteral::Lookup(Type::Pointer arg_t) -> std::optional<UnopCodegen *> {
  auto iterator =
      std::find_if(overloads.begin(),
                   overloads.end(),
                   [arg_t](const auto &overload) {
                     return arg_t == std::get<Type::Pointer>(overload);
                   });

  if (iterator == overloads.end()) {
    return {};
  }

  return {std::addressof(iterator->second)};
}
} // namespace pink
