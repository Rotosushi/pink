/**
 * @file BinopCodegen.h
 * @brief Header for class BinopCodegen
 * @version 0.1
 */
#pragma once
#include "llvm/IR/Value.h"

#include "aux/Error.h"
#include "aux/Outcome.h"

#include "type/Type.h"

namespace pink {
// This forward declaration is needed to break the circular dependancy
// that would be created by the fact that pink::Environment requires
// the definition of pink::BinopTable, and pink::BinopTable needs
// the definition of pink::BinopLiteral, and pinkBinopLiteral needs
// the definition of pink::BinopCodegen, which needs the definition of
// pink::Environment.
class Environment;

/**
 * @brief pointer to a function which can generate the code of an implementation
 * of a binary operation
 *
 */
using BinopCodegenFn = Outcome<llvm::Value *, Error> (*)(
    llvm::Type *lty, llvm::Value *left, llvm::Type *rty, llvm::Value *right,
    const Environment &env);

/**
 * @brief Represents a generator for the implementation of a particular Binop
 *
 */
class BinopCodegen {
public:
  /**
   * @brief The result type of the generator function
   *
   */
  Type *result_type;

  /**
   * @brief The function which can generate the implementation of this Binop
   *
   */
  BinopCodegenFn generate;

  BinopCodegen() = delete;

  /**
   * @brief Construct a new Binop Codegen
   *
   * @param other the other object to copy
   */
  BinopCodegen(const BinopCodegen &other) = default;

  /**
   * @brief Construct a new Binop Codegen
   *
   * @param ret_t the return type of the binary operator
   * @param fn_p the generator function
   */
  BinopCodegen(Type *ret_t, BinopCodegenFn fn_p);
};
} // namespace pink
