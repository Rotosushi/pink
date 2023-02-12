/**
 * @file BinopTable.h
 * @brief Header for class BinopTable
 * @version 0.1
 */
#pragma once
#include <memory>   // std::unique_ptr
#include <optional> // std::optional
#include <utility>  // std::pair<>

#include "llvm/ADT/DenseMap.h" // llvm::DenseMap<>

#include "aux/StringInterner.h"

#include "ops/BinopLiteral.h"

namespace pink {
/**
 * @brief Represents the table of all known binops
 */
class BinopTable {
private:
  llvm::DenseMap<InternedString, std::unique_ptr<BinopLiteral>> table;

public:
  BinopTable() noexcept                                            = default;
  ~BinopTable() noexcept                                           = default;
  BinopTable(const BinopTable &other) noexcept                     = delete;
  BinopTable(BinopTable &&other) noexcept                          = default;
  auto operator=(const BinopTable &other) noexcept -> BinopTable & = delete;
  auto operator=(BinopTable &&other) noexcept -> BinopTable      & = default;

  auto Register(InternedString opr,
                Precedence     precedence,
                Associativity  associativity) -> BinopLiteral *;

  auto Register(InternedString opr,
                Precedence     precedence,
                Associativity  associativity,
                Type::Pointer  left_t,
                Type::Pointer  right_t,
                Type::Pointer  ret_t,
                BinopCodegenFn fn_p) -> BinopLiteral *;

  void Unregister(InternedString opr);

  auto Lookup(InternedString opr) -> std::optional<BinopLiteral *>;
};
} // namespace pink
