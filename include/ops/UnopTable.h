/**
 * @file UnopTable.h
 * @brief Header for class UnopTable
 * @version 0.1
 */
#pragma once
#include <memory>   // std::unique_ptr
#include <optional> // std::optional
#include <utility>  // std::pair
#include <vector>   // std::vector

#include "llvm/IR/Value.h"

#include "type/Type.h"

#include "aux/Map.h"            // pink::Map<K, V>
#include "aux/StringInterner.h" // pink::InternedString

namespace pink {
class Environment;

class UnopCodegen {
public:
  /**
   * @brief pointer to a function which can be used
   * to generate an llvm IR implementation of a given unop
   */
  using Function = llvm::Value *(*)(llvm::Value *term, Environment &env);

private:
  Type::Pointer return_type;
  Function      function;

public:
  UnopCodegen() noexcept
      : return_type{nullptr},
        function{nullptr} {}
  UnopCodegen(Type::Pointer return_type, Function function) noexcept
      : return_type{return_type},
        function{function} {}
  ~UnopCodegen() noexcept                                            = default;
  UnopCodegen(const UnopCodegen &other) noexcept                     = default;
  UnopCodegen(UnopCodegen &&other) noexcept                          = default;
  auto operator=(const UnopCodegen &other) noexcept -> UnopCodegen & = default;
  auto operator=(UnopCodegen &&other) noexcept -> UnopCodegen      & = default;

  [[nodiscard]] auto GetReturnType() const noexcept -> Type::Pointer {
    return return_type;
  }

  [[nodiscard]] auto GetFunction() const noexcept -> Function {
    return function;
  }
};

/**
 * @brief Represents a single unary operator
 */
class UnopLiteral : public Map<Type::Pointer, UnopCodegen> {};
/**
 * @brief Represents the set of all known unary operators
 */
class UnopTable : public Map<InternedString, UnopLiteral> {};
} // namespace pink
