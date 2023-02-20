
#include "ops/UnopTable.h"

namespace pink {

auto UnopTable::Register(InternedString opr) -> UnopLiteral * {
  auto iterator =
      std::find_if(table.begin(), table.end(), [opr](const auto &binop) {
        return opr == std::get<InternedString>(binop);
      });

  if (iterator != table.end()) {
    return std::addressof(iterator->second);
  }

  auto &pair = table.emplace_back(opr, UnopLiteral{});
  return std::addressof(pair.second);
}

auto UnopTable::Register(InternedString opr,
                         Type::Pointer  arg_t,
                         Type::Pointer  ret_t,
                         UnopCodegenFn  fn_p) -> UnopLiteral * {
  auto iterator = std::find_if(
      table.begin(),
      table.end(),
      [opr](const std::pair<InternedString, UnopLiteral> &implementation) {
        return opr == std::get<InternedString>(implementation);
      });

  if (iterator != table.end()) {
    iterator->second.Register(arg_t, ret_t, fn_p);
    return std::addressof(iterator->second);
  }

  auto &pair = table.emplace_back(opr, UnopLiteral{arg_t, ret_t, fn_p});
  return std::addressof(pair.second);
}

auto UnopTable::Lookup(InternedString opr) -> std::optional<UnopLiteral *> {
  auto iterator = std::find_if(
      table.begin(),
      table.end(),
      [opr](const std::pair<InternedString, UnopLiteral> &implementation) {
        return opr == std::get<InternedString>(implementation);
      });

  if (iterator == table.end()) {
    return {};
  }

  return {std::addressof(iterator->second)};
}
} // namespace pink
