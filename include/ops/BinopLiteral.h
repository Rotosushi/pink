/**
 * @file BinopLiteral.h
 * @brief Header for class BinopLiteral
 * @version 0.1
 */
#pragma once
#include <optional> // std::optional
#include <utility>  // std::pair
#include <vector>   // std::vector

// #include "llvm/ADT/DenseMap.h"

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
  using Value = BinopCodegen;

private:
  static constexpr auto              initial_size = 5;
  std::vector<std::pair<Key, Value>> overloads;
  Precedence                         precedence;
  Associativity                      associativity;

public:
  BinopLiteral() noexcept  = delete;
  ~BinopLiteral() noexcept = default;
  BinopLiteral(Precedence precedence, Associativity associativity) noexcept;
  BinopLiteral(Precedence     precedence,
               Associativity  associativity,
               Type::Pointer  left_t,
               Type::Pointer  right_t,
               Type::Pointer  ret_t,
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

  auto Register(Type::Pointer  left_t,
                Type::Pointer  right_t,
                Type::Pointer  ret_t,
                BinopCodegenFn fn_p) -> BinopCodegen *;

  auto Lookup(Type::Pointer left_t, Type::Pointer right_t)
      -> std::optional<BinopCodegen *>;
};
} // namespace pink
