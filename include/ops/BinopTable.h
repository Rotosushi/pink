/**
 * @file BinopTable.h
 * @brief Header for class BinopTable
 * @version 0.1
 */
#pragma once
#include <memory>  // std::unique_ptr
#include <utility> // std::pair<>

#include "llvm/ADT/APInt.h"    // llvm::Optional<>
#include "llvm/ADT/DenseMap.h" // llvm::DenseMap<>

#include "aux/StringInterner.h"

#include "ops/BinopLiteral.h"

namespace pink {
/**
 * @brief Represents the table of all known binops
 *
 */
class BinopTable {
private:
  /**
   * @brief the table of registered binops
   *
   */
  llvm::DenseMap<InternedString, std::unique_ptr<BinopLiteral>> table;

public:
  /**
   * @brief Construct a new Binop Table
   *
   */
  BinopTable() = default;

  /**
   * @brief Destroy the Binop Table
   *
   */
  ~BinopTable() = default;

  /**
   * @brief Register a new binop in the table, with no implementations.
   *
   * If the operator already exists, this function simply returns the existing
   * binop
   *
   * @param opr the binary operator
   * @param precedence  the binops precedence
   * @param associativity  the binops associativity
   * @return std::pair<InternedString, BinopLiteral*> the binop that was added
   * to the table
   */
  auto Register(InternedString opr, Precedence precedence,
                Associativity associativity)
      -> std::pair<InternedString, BinopLiteral *>;

  /**
   * @brief Register a new binop in the table, with one implementation
   *
   * @param opr the binary operator
   * @param precedence the binops precedence
   * @param associativity the binops associativity
   * @param left_t the left argument Type of the implementation
   * @param right_t the right argument Type of the implementation
   * @param ret_t the return Type of the implementation
   * @param fn_p the function which generates the implementation.
   * @return std::pair<InternedString, BinopLiteral*> the binop that was added
   * to the table
   */
  auto Register(InternedString opr, Precedence precedence,
                Associativity associativity, Type *left_t, Type *right_t,
                Type *ret_t, BinopCodegenFn fn_p)
      -> std::pair<InternedString, BinopLiteral *>;

  /**
   * @brief Remove an existing binop from the table
   *
   * if the op is not already registered, does nothing.
   *
   * @param opr the op to remove from the table
   */
  void Unregister(InternedString opr);

  /**
   * @brief Lookup an existing binop in the table
   *
   * @param opr the binop to lookup
   * @return llvm::Optional<std::pair<InternedString, BinopLiteral*>> if
   * has_value, then the binop from the table, otherwise nothing.
   */
  auto Lookup(InternedString opr)
      -> llvm::Optional<std::pair<InternedString, BinopLiteral *>>;
};
} // namespace pink
