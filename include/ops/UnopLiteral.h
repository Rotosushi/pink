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
  /**
   * @brief a [table] of all implementations of this unop
   *
   * [table]: https://llvm.org/doxygen/DenseMap_8h.html "table"
   *
   */

  llvm::DenseMap<Type *, std::unique_ptr<UnopCodegen>> overloads;

public:
  /**
   * @brief Construct a new Unop Literal, with no implementations
   *
   */
  UnopLiteral() = default;

  /**
   * @brief Construct a new Unop Literal, with one implementation
   *
   * @param arg_t the argument Type for this unop
   * @param ret_t the return Type for this unop
   * @param fn_p the generator function for this unop
   */
  UnopLiteral(Type *arg_t, Type *ret_t, UnopCodegenFn fn_p);

  /**
   * @brief Destroy the Unop Literal
   *
   */
  ~UnopLiteral() = default;

  /**
   * @brief Register a new implementation of this Unop
   *
   * @param arg_t the argument Type of the new implementation
   * @param ret_t the return Type of the new implementation
   * @param fn_p the generator function for the new implementation
   * @return std::pair<Type*, UnopCodegen*> The new implementation
   */
  auto Register(Type *arg_t, Type *ret_t, UnopCodegenFn fn_p)
      -> std::pair<Type *, UnopCodegen *>;

  /**
   * @brief Remove an implementation of this unop
   *
   * @param arg_t the argument type of the implementation to remove
   */
  void Unregister(Type *arg_t);

  /**
   * @brief Lookup an implementation of this unop for the given argument Type.
   *
   * @param arg_t the argument Type to lookup
   * @return llvm::Optional<std::pair<Type*, UnopCodegen*>> if has_value the
   * implementation for the given Type, otherwise nothing.
   */
  virtual auto Lookup(Type *arg_t) -> llvm::Optional<std::pair<Type *, UnopCodegen *>>;
};

} // namespace pink
