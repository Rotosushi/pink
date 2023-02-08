
#include "ops/BinopLiteral.h"

namespace pink {
BinopLiteral::BinopLiteral(Precedence    precedence,
                           Associativity associativity) noexcept
    : precedence(precedence), associativity(associativity) {}

BinopLiteral::BinopLiteral(Precedence precedence, Associativity associativity,
                           Type::Pointer left_t, Type::Pointer right_t,
                           Type::Pointer ret_t, BinopCodegenFn fn_p)
    : precedence(precedence), associativity(associativity) {
  overloads.insert(std::make_pair(std::make_pair(left_t, right_t),
                                  std::make_unique<BinopCodegen>(ret_t, fn_p)));
}

auto BinopLiteral::Register(Type::Pointer left_t, Type::Pointer right_t,
                            Type::Pointer ret_t, BinopCodegenFn fn_p)
    -> std::pair<Key, BinopCodegen *> {
  auto iter = overloads.find(std::make_pair(left_t, right_t));

  if (iter == overloads.end()) {
    auto pair = overloads.insert(
        std::make_pair(std::make_pair(left_t, right_t),
                       std::make_unique<BinopCodegen>(ret_t, fn_p)));
    return std::make_pair(pair.first->first, pair.first->second.get());
  }

  return std::make_pair(iter->first, iter->second.get());
}

void BinopLiteral::Unregister(Type::Pointer left_t, Type::Pointer right_t) {
  auto iter = overloads.find(std::make_pair(left_t, right_t));

  if (iter != overloads.end()) {
    overloads.erase(iter);
  }
}

auto BinopLiteral::Lookup(Type::Pointer left_t, Type::Pointer right_t)
    -> std::optional<std::pair<Key, BinopCodegen *>> {
  auto iter = overloads.find(std::make_pair(left_t, right_t));

  if (iter == overloads.end()) {
    return {};
  }
  return {std::make_pair(iter->first, iter->second.get())};
}
} // namespace pink
