#include "ops/BinopTable.h"

namespace pink {

auto BinopTable::Register(InternedString opr,
                          Precedence     precedence,
                          Associativity  associativity) -> BinopLiteral * {
  auto iterator =
      std::find_if(table.begin(), table.end(), [opr](const auto &binop) {
        return opr == std::get<InternedString>(binop);
      });

  if (iterator == table.end()) {
    auto &pair =
        table.emplace_back(opr, BinopLiteral{precedence, associativity});
    return std::addressof(pair.second);
  }

  return std::addressof(iterator->second);
}

auto BinopTable::Register(InternedString opr,
                          Precedence     precedence,
                          Associativity  associativity,
                          Type::Pointer  left_t,
                          Type::Pointer  right_t,
                          Type::Pointer  ret_t,
                          BinopCodegenFn fn_p) -> BinopLiteral * {
  auto iterator =
      std::find_if(table.begin(), table.end(), [opr](const auto &binop) {
        return opr == std::get<InternedString>(binop);
      });

  if (iterator == table.end()) {
    auto &pair = table.emplace_back(
        opr,
        BinopLiteral{precedence, associativity, left_t, right_t, ret_t, fn_p});
    return std::addressof(pair.second);
  }

  iterator->second.Register(left_t, right_t, ret_t, fn_p);
  return std::addressof(iterator->second);
}

auto BinopTable::Lookup(InternedString opr) -> std::optional<BinopLiteral *> {
  auto iterator =
      std::find_if(table.begin(), table.end(), [opr](const auto &binop) {
        return opr == std::get<InternedString>(binop);
      });

  if (iterator == table.end()) {
    return {};
  }
  return {std::addressof(iterator->second)};
}
} // namespace pink
