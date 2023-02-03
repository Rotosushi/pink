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
class Environment;

using BinopCodegenFn = llvm::Value *(*)(llvm::Type *lty, llvm::Value *left,
                                        llvm::Type *rty, llvm::Value *right,
                                        Environment &env);

/**
 * @brief Represents an implementation of a particular Binop
 */
class BinopCodegen {
  Type::Pointer  result_type;
  BinopCodegenFn generate;

public:
  BinopCodegen() noexcept  = delete;
  ~BinopCodegen() noexcept = default;
  BinopCodegen(Type::Pointer ret_t, BinopCodegenFn fn_p) noexcept
      : result_type(ret_t), generate(fn_p) {}
  BinopCodegen(const BinopCodegen &other) noexcept = default;
  BinopCodegen(BinopCodegen &&other) noexcept      = default;
  auto operator=(const BinopCodegen &other) noexcept
      -> BinopCodegen                                           & = default;
  auto operator=(BinopCodegen &&other) noexcept -> BinopCodegen & = default;

  [[nodiscard]] auto GetReturnType() const -> Type::Pointer {
    return result_type;
  }
  [[nodiscard]] auto GetGenerateFn() const -> BinopCodegenFn { return generate; }
};
} // namespace pink
