/**
 * @file IntType.h
 * @brief Header for IntType
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
class IntType : public Type {
public:
  /**
   * @brief Construct a new IntType
   *
   */
  IntType();

  /**
   * @brief Destroy the IntType
   *
   */
  ~IntType() override = default;

  /**
   * @brief Implements LLVM style [RTTI] for this class
   *
   * @param type the type to be tested
   * @return true if type *is* an instance of IntType
   * @return false if type *is not* an instance of IntType
   */
  static auto classof(const Type *type) -> bool;

  /**
   * @brief Computes if other is equivalent to this IntType
   *
   * @param other the other type
   * @return true if other *is* equivalent to this IntType
   * @return false if other *is not* equivalent to this IntType
   */
  auto EqualTo(Type *other) const -> bool override;

  /**
   * @brief Compute the cannonical string representation of this IntType
   *
   * @return std::string the string representation
   */
  [[nodiscard]] auto ToString() const -> std::string override;

  /**
   * @brief Compute the llvm::Type equivalent to IntType
   *
   * @param env the environment of this compilation unit
   * @return Outcome<llvm::Type*, Error> if true the llvm::Type equivalent to
   * IntType, if false then the Error encountered.
   */
  [[nodiscard]] auto Codegen(const Environment &env) const
      -> Outcome<llvm::Type *, Error> override;
};
} // namespace pink
