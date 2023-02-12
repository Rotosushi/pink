
#include "ops/BinopLiteral.h"

namespace pink {
BinopLiteral::BinopLiteral(Precedence    precedence,
                           Associativity associativity) noexcept
    : overloads(initial_size),
      precedence(precedence),
      associativity(associativity) {}

BinopLiteral::BinopLiteral(Precedence     precedence,
                           Associativity  associativity,
                           Type::Pointer  left_t,
                           Type::Pointer  right_t,
                           Type::Pointer  ret_t,
                           BinopCodegenFn fn_p)
    : overloads(initial_size),
      precedence(precedence),
      associativity(associativity) {
  overloads.insert(std::make_pair(std::make_pair(left_t, right_t),
                                  std::make_unique<BinopCodegen>(ret_t, fn_p)));
}

auto BinopLiteral::Register(Type::Pointer  left_t,
                            Type::Pointer  right_t,
                            Type::Pointer  ret_t,
                            BinopCodegenFn fn_p) -> BinopCodegen * {
  auto iter = overloads.find(std::make_pair(left_t, right_t));

  if (iter == overloads.end()) {
    auto pair = overloads.insert(
        std::make_pair(std::make_pair(left_t, right_t),
                       std::make_unique<BinopCodegen>(ret_t, fn_p)));
    return pair.first->second.get();
  }

  return iter->second.get();
}

void BinopLiteral::Unregister(Type::Pointer left_t, Type::Pointer right_t) {
  auto iter = overloads.find(std::make_pair(left_t, right_t));

  if (iter != overloads.end()) {
    overloads.erase(iter);
  }
}

auto BinopLiteral::Lookup(Type::Pointer left_t, Type::Pointer right_t)
    -> std::optional<BinopCodegen *> {
  auto iter = overloads.find(std::make_pair(left_t, right_t));

  if (iter == overloads.end()) {
    return {};
  }
  return {iter->second.get()};
}
} // namespace pink
