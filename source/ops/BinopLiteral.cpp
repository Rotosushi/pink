
#include "ops/BinopLiteral.h"

namespace pink {
BinopLiteral::BinopLiteral(Precedence    precedence,
                           Associativity associativity) noexcept
    : precedence(precedence),
      associativity(associativity) {}

BinopLiteral::BinopLiteral(Precedence     precedence,
                           Associativity  associativity,
                           Type::Pointer  left_t,
                           Type::Pointer  right_t,
                           Type::Pointer  ret_t,
                           BinopCodegenFn fn_p)
    : precedence(precedence),
      associativity(associativity) {
  overloads.emplace_back(std::make_pair(left_t, right_t),
                         BinopCodegen{ret_t, fn_p});
}

auto BinopLiteral::Register(Type::Pointer  left_t,
                            Type::Pointer  right_t,
                            Type::Pointer  ret_t,
                            BinopCodegenFn fn_p) -> BinopCodegen * {
  auto iterator = std::find_if(overloads.begin(),
                               overloads.end(),
                               [left_t, right_t](const auto &overload) {
                                 return std::make_pair(left_t, right_t) ==
                                        std::get<BinopLiteral::Key>(overload);
                               });

  if (iterator == overloads.end()) {
    auto &pair = overloads.emplace_back(std::make_pair(left_t, right_t),
                                        BinopCodegen{ret_t, fn_p});
    return std::addressof(std::get<BinopCodegen>(pair));
  }

  return std::addressof(std::get<BinopCodegen>(*iterator));
}

auto BinopLiteral::Lookup(Type::Pointer left_t, Type::Pointer right_t)
    -> std::optional<BinopCodegen *> {
  auto iterator = std::find_if(overloads.begin(),
                               overloads.end(),
                               [left_t, right_t](const auto &overload) {
                                 return std::make_pair(left_t, right_t) ==
                                        std::get<BinopLiteral::Key>(overload);
                               });

  if (iterator == overloads.end()) {
    return {};
  }
  return {std::addressof(std::get<BinopCodegen>(*iterator))};
}
} // namespace pink
