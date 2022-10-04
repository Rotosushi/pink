/**
 * @file BoolType.h
 * @brief Header for class BoolType
 * @version 0.1
 */
#pragma once
#include <string>

#include "type/Type.h"

namespace pink {
/**
 * @brief Represents the Type of Bool
 *
 */
class BoolType : public Type {
public:
  /**
   * @brief Construct a new Bool Type
   *
   */
  BoolType();

  /**
   * @brief Destroy the Bool Type
   *
   */
  ~BoolType() override = default;

  /**
   * @brief Implements LLVM style [RTTI] for this class
   *
   * [RTTI]: https://llvm.org/docs/HowToSetUpLLVMStyleRTTI.html "RTTI"
   *
   * @param type the type being tested
   * @return true if type *is* an instance of BoolType
   * @return false if type *is not* an instance of BoolType
   */
  static auto classof(const Type *type) -> bool;

  /**
   * @brief Compute if this BoolType is equivalent to the other type
   *
   * @param other the other type
   * @return true if other *is* equivalent to BoolType
   * @return false if other *is not* equivalent to BoolType
   */
  auto EqualTo(Type *other) const -> bool override;

  /**
   * @brief Compute the cannonical string representation of this BoolType
   *
   * @return std::string the string representation
   */
  [[nodiscard]] auto ToString() const -> std::string override;

  /**
   * @brief Compute the LLVM equivalent of this BoolType
   *
   * @param env the environment of this compilation unit
   * @return Outcome<llvm::Type*, Error> if true the llvm::Type of this
   * BoolType, if false the Error encountered.
   */
  [[nodiscard]] auto Codegen(const Environment &env) const
      -> Outcome<llvm::Type *, Error> override;
};
} // namespace pink
