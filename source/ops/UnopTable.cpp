
#include "ops/UnopTable.h"

namespace pink {

auto UnopTable::Register(InternedString opr)
    -> std::pair<InternedString, UnopLiteral *> {
  auto iter = table.find(opr);

  if (iter != table.end()) {
    return std::make_pair(iter->first, iter->second.get());
  }

  auto pair =
      table.insert(std::make_pair(opr, std::make_unique<UnopLiteral>()));
  return std::make_pair(pair.first->first, pair.first->second.get());
}

auto UnopTable::Register(InternedString opr, Type::Pointer arg_t,
                         Type::Pointer ret_t, UnopCodegenFn fn_p)
    -> std::pair<InternedString, UnopLiteral *> {
  auto iter = table.find(opr);

  if (iter != table.end()) {
    iter->second->Register(arg_t, ret_t, fn_p);
    return std::make_pair(iter->first, iter->second.get());
  }

  auto pair = table.insert(
      std::make_pair(opr, std::make_unique<UnopLiteral>(arg_t, ret_t, fn_p)));
  return std::make_pair(pair.first->first, pair.first->second.get());
}

void UnopTable::Unregister(InternedString opr) {
  auto iter = table.find(opr);

  if (iter != table.end()) {
    table.erase(iter);
  }
}

auto UnopTable::Lookup(InternedString opr)
    -> llvm::Optional<std::pair<InternedString, UnopLiteral *>> {
  auto iter = table.find(opr);

  if (iter == table.end()) {
    return {};
  }

  return {std::make_pair(iter->first, iter->second.get())};
}
} // namespace pink
