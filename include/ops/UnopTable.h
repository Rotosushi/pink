/**
 * @file UnopTable.h
 * @brief Header for class UnopTable
 * @version 0.1
 */
#pragma once
#include <memory>   // std::unique_ptr
#include <optional> // std::optional
#include <utility>  // std::pair

#include "llvm/ADT/DenseMap.h"

#include "aux/StringInterner.h"

#include "ops/UnopLiteral.h"

namespace pink {
/**
 * @brief Represents the set of all known unary operators
 */
class UnopTable {
private:
  static constexpr auto initial_size = 5;

  llvm::DenseMap<InternedString, std::unique_ptr<UnopLiteral>> map;

public:
  UnopTable() noexcept
      : map(initial_size){};
  ~UnopTable() noexcept                                          = default;
  UnopTable(const UnopTable &other) noexcept                     = delete;
  UnopTable(UnopTable &&other) noexcept                          = default;
  auto operator=(const UnopTable &other) noexcept -> UnopTable & = delete;
  auto operator=(UnopTable &&other) noexcept -> UnopTable      & = default;

  auto Register(InternedString opr) -> UnopLiteral *;
  auto Register(InternedString opr,
                Type::Pointer  arg_t,
                Type::Pointer  ret_t,
                UnopCodegenFn  fn_p) -> UnopLiteral *;
  void Unregister(InternedString opr);
  auto Lookup(InternedString opr) -> std::optional<UnopLiteral *>;
};
} // namespace pink
