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
 */
using UnopCodegenFn = llvm::Value *(*)(llvm::Value       *term,
                                       const Environment &env);

class UnopCodegen {
private:
  Type::Pointer result_type;
  UnopCodegenFn generate;

public:
  UnopCodegen()           = delete;
  ~UnopCodegen() noexcept = default;
  UnopCodegen(Type::Pointer return_type, UnopCodegenFn gen) noexcept
      : result_type(return_type), generate(gen) {}
  UnopCodegen(const UnopCodegen &other) noexcept                     = default;
  UnopCodegen(UnopCodegen &&other) noexcept                          = default;
  auto operator=(const UnopCodegen &other) noexcept -> UnopCodegen & = default;
  auto operator=(UnopCodegen &&other) noexcept -> UnopCodegen      & = default;

  [[nodiscard]] auto GetReturnType() const noexcept -> Type::Pointer {
    return result_type;
  }

  [[nodiscard]] auto GetGenerateFn() const noexcept -> UnopCodegenFn {
    return generate;
  }
};
} // namespace pink
