/**
 * @file NilType.h
 * @brief Header for class NilType
 * @version 0.1
 */
#pragma once
#include <string>

#include "type/Type.h"

namespace pink {
/**
 * @brief Represents the Type of Nil
 *
 */
class NilType : public Type {
public:
  /**
   * @brief Construct a new NilType
   *
   */
  NilType();

  /**
   * @brief Destroy the NilType
   *
   */
  ~NilType() override = default;

  NilType(const NilType &other) = default;

  NilType(NilType &&other) = default;

  auto operator=(const NilType &other) -> NilType & = default;

  auto operator=(NilType &&other) -> NilType & = default;

  /**
   * @brief Implements LLVM style [RTTI] for this class
   *
   * [RTTI]: https://llvm.org/docs/HowToSetUpLLVMStyleRTTI.html "RTTI"
   *
   * @param type the type being tested
   * @return true if type *is* an instance of NilType
   * @return false if type *is not* an instance of NilType
   */
  static auto classof(const Type *type) -> bool;

  /**
   * @brief Compute if other Type is equivalent to this NilType
   *
   * @param other the other type
   * @return true if other *is* equivalent to this NilType
   * @return false if other *is not* equivalent to this NilType
   */
  auto EqualTo(Type *other) const -> bool override;

  /**
   * @brief Compute the cannonical string representation of this NilType
   *
   * @return std::string the string representation
   */
  [[nodiscard]] auto ToString() const -> std::string override;

  /**
   * @brief Compute the llvm::Type equivalent to NilType
   *
   * @param env the environment of this compilation unit
   * @return Outcome<llvm::Type*, Error> if true the llvm::Type equivalent to
   * NilType, if false the Error encountered
   */
  [[nodiscard]] auto Codegen(const Environment &env) const
      -> llvm::Type * override;
};
} // namespace pink
