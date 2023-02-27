#include "ops/BinopTable.h"

namespace pink {

auto BinopTable::Register(InternedString opr,
                          Precedence     precedence,
                          Associativity  associativity) -> BinopLiteral * {
  for (auto &binop : table) {
    if (binop.first == opr) {
      return std::addressof(binop.second);
    }
  }
  auto &pair = table.emplace_back(
      opr,
      BinopLiteral{precedence, associativity});
  return std::addressof(pair.second);
}

auto BinopTable::Register(InternedString opr,
                          Precedence     precedence,
                          Associativity  associativity,
                          Type::Pointer  left_t,
                          Type::Pointer  right_t,
                          Type::Pointer  ret_t,
                          BinopCodegenFn fn_p) -> BinopLiteral * {
  for (auto &binop : table) {
    if (binop.first == opr) {
      binop.second.Register(left_t, right_t, ret_t, fn_p);
      return std::addressof(binop.second);
    }
  }
  auto &pair = table.emplace_back(
      opr,
      BinopLiteral{precedence, associativity, left_t, right_t, ret_t, fn_p});
  return std::addressof(pair.second);
}

auto BinopTable::Lookup(InternedString opr) -> std::optional<BinopLiteral *> {
  for (auto &binop : table) {
    if (binop.first == opr) {
      return {std::addressof(binop.second)};
    }
  }
  return {};
}
} // namespace pink
