/**
 * @file IntegerType.h
 * @brief Header for IntegerType
 * @version 0.1
 */
#pragma once
#include <string>

#include "type/Type.h"

namespace pink {
/**
 * @brief Represents the Type of Int
 *
 */
class IntegerType : public Type {
public:
  /**
   * @brief Construct a new IntegerType
   *
   */
  IntegerType();

  /**
   * @brief Destroy the IntegerType
   *
   */
  ~IntegerType() override = default;

  IntegerType(const IntegerType &other) = default;

  IntegerType(IntegerType &&other) = default;

  auto operator=(const IntegerType &other) -> IntegerType & = default;

  auto operator=(IntegerType &&other) -> IntegerType & = default;

  /**
   * @brief Implements LLVM style [RTTI] for this class
   *
   * @param type the type to be tested
   * @return true if type *is* an instance of IntegerType
   * @return false if type *is not* an instance of IntegerType
   */
  static auto classof(const Type *type) -> bool;

  /**
   * @brief Computes if other is equivalent to this IntegerType
   *
   * @param other the other type
   * @return true if other *is* equivalent to this IntegerType
   * @return false if other *is not* equivalent to this IntegerType
   */
  auto EqualTo(Type *other) const -> bool override;

  /**
   * @brief Compute the cannonical string representation of this IntegerType
   *
   * @return std::string the string representation
   */
  [[nodiscard]] auto ToString() const -> std::string override;

  /**
   * @brief Compute the llvm::Type equivalent to IntegerType
   *
   * @param env the environment of this compilation unit
   * @return Outcome<llvm::Type*, Error> if true the llvm::Type equivalent to
   * IntegerType, if false then the Error encountered.
   */
  [[nodiscard]] auto ToLLVM(const Environment &env) const
      -> llvm::Type * override;
};
} // namespace pink
