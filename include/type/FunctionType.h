/**
 * @file FunctionType.h
 * @brief Header for class FunctionType
 * @version 0.1
 */
#pragma once
#include <vector>

#include "type/Type.h"

namespace pink {
/**
 * @brief Represents the Type of a Function
 *
 */
class FunctionType : public Type {
public:
  /**
   * @brief the return Type of the Function
   *
   */
  Type *result;

  /**
   * @brief The argument Types of the Function
   *
   */
  std::vector<Type *> arguments;

  /**
   * @brief Construct a new FunctionType
   *
   * @param return_type the return Type of the Function
   * @param arg_types the argument Types of the Function
   */
  FunctionType(Type *return_type, const std::vector<Type *> &arg_types);

  /**
   * @brief Destroy the FunctionType
   *
   */
  ~FunctionType() override = default;

  FunctionType(const FunctionType &other) = default;

  FunctionType(FunctionType &&other) = default;

  auto operator=(const FunctionType &other) -> FunctionType & = default;

  auto operator=(FunctionType &&other) -> FunctionType & = default;

  /**
   * @brief Implements LLVM style [RTTI] for this clas
   *
   * [RTTI]: https://llvm.org/docs/HowToSetUpLLVMStyleRTTI.html "RTTI"
   *
   * @param type the Type being tested
   * @return true if type *is* an instance of FunctionType
   * @return false if type *is not* an instance of FunctionType
   */
  static auto classof(const Type *type) -> bool;

  /**
   * @brief Compute if this FunctionType is equivalent to the other Type
   *
   * @param other the other type
   * @return true if other is equivalent to this FunctionType
   * @return false if other is not equivalent to this FunctionType
   */
  auto EqualTo(Type *other) const -> bool override;

  /**
   * @brief Compute the cannonical string representation of this FunctionType
   *
   * @return std::string the string representation
   */
  [[nodiscard]] auto ToString() const -> std::string override;

  /**
   * @brief Compute the llvm::Type equal to this FunctionType
   *
   * @param env the environment of this compilation environment
   * @return Outcome<llvm::Type*, Error> if true the llvm::Type equal to the
   * FunctionType, if false then the Error encountered
   */
  [[nodiscard]] auto Codegen(const Environment &env) const
      -> Outcome<llvm::Type *, Error> override;
};
} // namespace pink
