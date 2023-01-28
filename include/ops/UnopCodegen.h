/**
 * @file UnopCodegen.h
 * @brief Header for class UnopCodegen
 * @version 0.1
 */
#pragma once

#include "llvm/IR/Value.h"

#include "type/Type.h"

namespace pink {
class Environment;

/**
 * @brief pointer to a function which can be used to generate an implementation
 * of a given unop
 *
 * I suppose that technically, we are going to
 * catch any errors before the point of calling
 * an actual generator expression. Thus we should
 * not need an Outcome type to wrap any potential
 * errors, as the body of a generator is always
 * going to succeed.
 */
using UnopCodegenFn = llvm::Value *(*)(llvm::Value       *term,
                                       const Environment &env);

class UnopCodegen {
public:
  /**
   * @brief The result type of calling the generator function
   *
   */
  Type *result_type;

  /**
   * @brief a pointer to the generator function
   *
   */
  UnopCodegenFn generate;

  UnopCodegen()                         = delete;

  /**
   * @brief Construct a new Unop Codegen
   *
   * @param other the UnopCodegen to copy
   */
  UnopCodegen(const UnopCodegen &other) = default;

  /**
   * @brief Construct a new Unop Codegen
   *
   * @param return_type the return type of the generator function
   * @param gen the generator function
   */
  UnopCodegen(Type *return_type, UnopCodegenFn gen);
};
} // namespace pink
