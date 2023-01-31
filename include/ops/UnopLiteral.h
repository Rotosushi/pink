/**
 * @file UnopLiteral.h
 * @brief Header for class UnopLiteral
 * @version 0.1
 */
#pragma once
#include <memory>  // std::shared_ptr
#include <utility> // std::pair

#include "llvm/ADT/APInt.h"
#include "llvm/ADT/DenseMap.h"

#include "ops/UnopCodegen.h"

namespace pink {
/**
 * @brief Represents a single unary operator
 *
 */
class UnopLiteral {
private:
  llvm::DenseMap<Type::Pointer, std::unique_ptr<UnopCodegen>> overloads;

public:
  UnopLiteral() noexcept  = default;
  ~UnopLiteral() noexcept = default;
  UnopLiteral(Type::Pointer arg_t, Type::Pointer ret_t, UnopCodegenFn fn_p) {
    overloads.insert(
        std::make_pair(arg_t, std::make_unique<UnopCodegen>(ret_t, fn_p)));
  }
  UnopLiteral(const UnopLiteral &other) noexcept                     = delete;
  UnopLiteral(UnopLiteral &&other) noexcept                          = default;
  auto operator=(const UnopLiteral &other) noexcept -> UnopLiteral & = delete;
  auto operator=(UnopLiteral &&other) noexcept -> UnopLiteral      & = default;

  auto Register(Type::Pointer arg_t, Type::Pointer ret_t, UnopCodegenFn fn_p)
      -> std::pair<Type::Pointer, UnopCodegen *>;
  void Unregister(Type::Pointer arg_t);
  auto Lookup(Type::Pointer arg_t)
      -> llvm::Optional<std::pair<Type::Pointer, UnopCodegen *>>;
};

} // namespace pink
