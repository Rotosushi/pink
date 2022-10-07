
#include "ops/BinopLiteral.h"

namespace pink {
BinopLiteral::BinopLiteral(Precedence precedence, Associativity associativity)
    : precedence(precedence), associativity(associativity) {}

BinopLiteral::BinopLiteral(Precedence precedence, Associativity associativity,
                           Type *left_t, Type *right_t, Type *ret_t,
                           BinopCodegenFn fn_p)
    : precedence(precedence), associativity(associativity) {
  overloads.insert(std::make_pair(std::make_pair(left_t, right_t),
                                  std::make_unique<BinopCodegen>(ret_t, fn_p)));
}

auto BinopLiteral::NumOverloads() const -> unsigned { return overloads.size(); }

auto BinopLiteral::Register(Type *left_t, Type *right_t, Type *ret_t,
                            BinopCodegenFn fn_p)
    -> std::pair<std::pair<Type *, Type *>, BinopCodegen *> {
  auto iter = overloads.find(std::make_pair(left_t, right_t));

  if (iter == overloads.end()) {
    auto pair = overloads.insert(
        std::make_pair(std::make_pair(left_t, right_t),
                       std::make_unique<BinopCodegen>(ret_t, fn_p)));
    return std::make_pair(pair.first->first, pair.first->second.get());
  }

  return std::make_pair(iter->first, iter->second.get());
}

void BinopLiteral::Unregister(Type *left_t, Type *right_t) {
  auto iter = overloads.find(std::make_pair(left_t, right_t));

  if (iter != overloads.end()) {
    overloads.erase(iter);
  }
}

auto BinopLiteral::Lookup(Type *left_t, Type *right_t)
    -> llvm::Optional<std::pair<std::pair<Type *, Type *>, BinopCodegen *>> {
  auto iter = overloads.find(std::make_pair(left_t, right_t));

  if (iter == overloads.end()) {
    return {};
  }
  return {std::make_pair(iter->first, iter->second.get())};
}
} // namespace pink
