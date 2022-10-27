/**
 * @file VoidType.h
 * @brief Header for class VoidType
 * @version 0.1
 */
#pragma once

#include "type/Type.h"

namespace pink {
/**
 * @brief Represents the Type of Void
 *
 */
class VoidType : public Type {
public:
  /**
   * @brief Construct a new Void Type
   *
   */
  VoidType();

  /**
   * @brief Destroy the Void Type
   *
   */
  ~VoidType() override = default;

  VoidType(const VoidType &other) = default;

  VoidType(VoidType &&other) = default;

  auto operator=(const VoidType &other) -> VoidType & = default;

  auto operator=(VoidType &&other) -> VoidType & = default;

  /**
   * @brief Implements LLVM style [RTTI] for this class
   *
   * @param type the type being tested
   * @return true if type *is* an instance of VoidType
   * @return false if type *is not* an instance of VoidType
   */
  static auto classof(const Type *type) -> bool;

  /**
   * @brief Computes if other is equivalent to this VoidType
   *
   * @param other the other type
   * @return true if other *is* equivalent to this VoidType
   * @return false if other *is not* equivalent to this VoidType
   */
  auto EqualTo(Type *other) const -> bool override;

  /**
   * @brief Compute the cannonical string representation of this VoidType
   *
   * @return std::string the string representation
   */
  [[nodiscard]] auto ToString() const -> std::string override;

  /**
   * @brief Compute the llvm::Type equivalent of this VoidType
   *
   * @param env the environment of this compilation unit
   * @return Outcome<llvm::Type*, Error> if true the llvm::Type equivalent of
   * this VoidType, if false then the Error encountered
   */
  [[nodiscard]] auto ToLLVM(const Environment &env) const
      -> llvm::Type * override;
};
} // namespace pink
