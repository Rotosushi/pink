/**
 * @file UnopTable.h
 * @brief Header for class UnopTable
 * @version 0.1
 */
#pragma once
#include <memory>  // std::unique_ptr
#include <utility> // std::pair

#include "llvm/ADT/APInt.h"
#include "llvm/ADT/DenseMap.h"

#include "aux/StringInterner.h"

#include "ops/UnopLiteral.h"

namespace pink {
/**
 * @brief Represents the table of all known unary operators
 *
 */
class UnopTable {
private:
  /**
   * @brief The [table] of all known unops
   * [table]: https://llvm.org/doxygen/DenseMap_8h.html "table"
   */
  llvm::DenseMap<InternedString, std::unique_ptr<UnopLiteral>> table;

public:
  /**
   * @brief Construct a new Unop Table
   *
   */
  UnopTable() = default;

  /**
   * @brief Destroy the Unop Table
   *
   */
  ~UnopTable() = default;

  /**
   * @brief Register a new unary operator with no implementations
   *
   * @param opr the operator to register in the table
   * @return std::pair<InternedString, UnopLiteral*> the new operator
   */
  auto Register(InternedString opr) -> std::pair<InternedString, UnopLiteral *>;

  /**
   * @brief Register a new unary operator with one implementation
   *
   * @param opr the operator to register in the table
   * @param arg_t the argument Type of the implementation
   * @param ret_t the return Type of the implementation
   * @param fn_p the generator function of the implementation
   * @return std::pair<InternedString, UnopLiteral*> the new operator
   */
  auto Register(InternedString opr, Type *arg_t, Type *ret_t,
                UnopCodegenFn fn_p) -> std::pair<InternedString, UnopLiteral *>;

  /**
   * @brief Remove an operator from the table
   *
   * @param opr the operator to remove
   */
  void Unregister(InternedString opr);

  /**
   * @brief Lookup an operator from the table
   *
   * @param opr the operator to lookup
   * @return llvm::Optional<std::pair<InternedString, UnopLiteral*>> if
   * has_value, the operator from the table, otherwise nothing.
   */
  auto Lookup(InternedString opr)
      -> llvm::Optional<std::pair<InternedString, UnopLiteral *>>;
};
} // namespace pink
