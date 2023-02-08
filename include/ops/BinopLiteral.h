/**
 * @file BinopLiteral.h
 * @brief Header for class BinopLiteral
 * @version 0.1
 */
#pragma once
#include <memory>   // std::unique_ptr
#include <optional> // std::optional
#include <utility>  // std::pair

#include "llvm/ADT/DenseMap.h"

#include "ops/BinopCodegen.h"
#include "ops/PrecedenceAndAssociativity.h"

namespace pink {
/**
 * @brief Represents a single binary operator
 *
 */
class BinopLiteral {
public:
  using Key   = std::pair<Type::Pointer, Type::Pointer>;
  using Value = std::unique_ptr<BinopCodegen>;

private:
  llvm::DenseMap<Key, Value> overloads;
  Precedence                 precedence;
  Associativity              associativity;

public:
  BinopLiteral() noexcept  = delete;
  ~BinopLiteral() noexcept = default;
  BinopLiteral(Precedence precedence, Associativity associativity) noexcept;
  BinopLiteral(Precedence precedence, Associativity associativity,
               Type::Pointer left_t, Type::Pointer right_t, Type::Pointer ret_t,
               BinopCodegenFn fn_p);
  BinopLiteral(const BinopLiteral &other) noexcept = delete;
  BinopLiteral(BinopLiteral &&other) noexcept      = default;
  auto operator=(const BinopLiteral &other) noexcept -> BinopLiteral & = delete;
  auto operator=(BinopLiteral &&other) noexcept -> BinopLiteral & = default;

  [[nodiscard]] auto NumOverloads() const noexcept -> unsigned int {
    return overloads.size();
  }

  [[nodiscard]] auto GetPrecedence() const noexcept -> Precedence {
    return precedence;
  }

  [[nodiscard]] auto GetAssociativity() const noexcept -> Associativity {
    return associativity;
  }

  auto Register(Type::Pointer left_t, Type::Pointer right_t,
                Type::Pointer ret_t, BinopCodegenFn fn_p)
      -> std::pair<Key, BinopCodegen *>;
  void Unregister(Type::Pointer left_t, Type::Pointer right_t);

  auto Lookup(Type::Pointer left_t, Type::Pointer right_t)
      -> std::optional<std::pair<Key, BinopCodegen *>>;
};
} // namespace pink
