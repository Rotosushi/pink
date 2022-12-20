/**
 * @file BooleanType.h
 * @brief Header for class BooleanType
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
class BooleanType : public Type {
public:
  /**
   * @brief Construct a new Bool Type
   *
   */
  BooleanType();

  /**
   * @brief Destroy the Bool Type
   *
   */
  ~BooleanType() override = default;

  BooleanType(const BooleanType &other) = default;

  BooleanType(BooleanType &&other) = default;

  auto operator=(const BooleanType &other) -> BooleanType & = default;

  auto operator=(BooleanType &&other) -> BooleanType & = default;

  /**
   * @brief Implements LLVM style [RTTI] for this class
   *
   * [RTTI]: https://llvm.org/docs/HowToSetUpLLVMStyleRTTI.html "RTTI"
   *
   * @param type the type being tested
   * @return true if type *is* an instance of BooleanType
   * @return false if type *is not* an instance of BooleanType
   */
  static auto classof(const Type *type) -> bool;

  /**
   * @brief Compute if this BooleanType is equivalent to the other type
   *
   * @param other the other type
   * @return true if other *is* equivalent to BooleanType
   * @return false if other *is not* equivalent to BooleanType
   */
  auto EqualTo(Type *other) const -> bool override;

  /**
   * @brief Compute the cannonical string representation of this BooleanType
   *
   * @return std::string the string representation
   */
  [[nodiscard]] auto ToString() const -> std::string override;

  /**
   * @brief Compute the LLVM equivalent of this BooleanType
   *
   * @param env the environment of this compilation unit
   * @return Outcome<llvm::Type*, Error> if true the llvm::Type of this
   * BooleanType, if false the Error encountered.
   */
  [[nodiscard]] auto ToLLVM(const Environment &env) const
      -> llvm::Type * override;
};
} // namespace pink
