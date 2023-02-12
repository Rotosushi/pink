
#include "ops/UnopTable.h"

namespace pink {

auto UnopTable::Register(InternedString opr) -> UnopLiteral * {
  auto iter = map.find(opr);

  if (iter != map.end()) {
    return iter->second.get();
  }

  auto pair = map.insert(std::make_pair(opr, std::make_unique<UnopLiteral>()));
  return pair.first->second.get();
}

auto UnopTable::Register(InternedString opr,
                         Type::Pointer  arg_t,
                         Type::Pointer  ret_t,
                         UnopCodegenFn  fn_p) -> UnopLiteral * {
  auto iter = map.find(opr);

  if (iter != map.end()) {
    iter->second->Register(arg_t, ret_t, fn_p);
    return iter->second.get();
  }

  auto pair = map.insert(
      std::make_pair(opr, std::make_unique<UnopLiteral>(arg_t, ret_t, fn_p)));
  return pair.first->second.get();
}

void UnopTable::Unregister(InternedString opr) {
  auto iter = map.find(opr);

  if (iter != map.end()) {
    map.erase(iter);
  }
}

auto UnopTable::Lookup(InternedString opr) -> std::optional<UnopLiteral *> {
  auto iter = map.find(opr);

  if (iter == map.end()) {
    return {};
  }

  return {iter->second.get()};
}
} // namespace pink
