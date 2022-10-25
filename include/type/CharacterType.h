#pragma once
#include "type/Type.h"

namespace pink {
class CharacterType : public Type {
public:
  CharacterType();
  ~CharacterType() override = default;
  CharacterType(const CharacterType &other) = default;
  CharacterType(CharacterType &&other) = default;
  auto operator=(const CharacterType &other) -> CharacterType & = default;
  auto operator=(CharacterType &&other) -> CharacterType & = default;

  /**
   * @brief Implements LLVM style [RTTI] for this class
   *
   * [RTTI]: https://llvm.org/docs/HowToSetUpLLVMStyleRTTI.html "RTTI"
   *
   * @param type the type being tested
   * @return true if type *is* an instance of CharacterType
   * @return false if type *is not* an instance of CharacterType
   */
  static auto classof(const Type *type) -> bool;

  /**
   * @brief Computes if other is equivalent to this CharacterType
   *
   * @param other the other type
   * @return true if other *is* equivalent to this CharacterType
   * @return false if other  *is not* equivalent to this CharacterType
   */
  auto EqualTo(Type *other) const -> bool override;

  /**
   * @brief Compute the cannonical string representation of this CharacterType
   *
   * @return std::string the string representation
   */
  [[nodiscard]] auto ToString() const -> std::string override;

  /**
   * @brief Compute the llvm::Type equivalent to this CharacterType
   *
   * @param env the environment of this compilation unit
   * @return Outcome<llvm::Type *, Error> if true the llvm::Type
   */
  [[nodiscard]] auto Codegen(const Environment &env) const
      -> llvm::Type * override;
};
} // namespace pink