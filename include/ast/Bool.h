/**
 * @file Bool.h
 * @brief Header for class Bool
 * @version 0.1
 *
 */
#pragma once
#include <string>

#include "ast/Ast.h"

namespace pink {
/**
 * @brief [Bool](#Bool) is an [Ast](#Ast) node which represents a boolean value
 *
 * @headerfile Bool.h "ast/Bool.h"
 */
class Bool : public Ast {
private:
  /**
   * @brief returns [BoolType](#BoolType)
   *
   * This function never constructs an [Error](#Error)
   *
   * @param env The [Environment](#Environment) which is used to get a
   * [BoolType](#BoolType) from the [TypeInterner](#TypeInterner)
   * @return Outcome<Type*, Error> A [BoolType](#BoolType) from the
   * [TypeInterner](#TypeInterner) held in [env](#Environment)
   */
  [[nodiscard]] auto GetypeV(const Environment &env) const
      -> Outcome<Type *, Error> override;

public:
  /**
   * @brief represents the state of this boolean value
   *
   * can be either true, or false.
   */
  bool value;

  /**
   * @brief Construct a new Bool
   *
   * @param l The textual [location] of the bool
   * @param b The value of the bool
   */
  Bool(const Location &location, const bool value);

  /**
   * @brief Destroy the Bool
   */
  ~Bool() override = default;

  /**
   * @brief This function is used to implement llvm style [RTTI] for this node
   * kind
   *
   * [RTTI]: https://llvm.org/docs/HowToSetUpLLVMStyleRTTI.html "RTTI"
   *
   * @param a The Ast* being tested
   * @return true if a *is* an instance of [Bool](#Bool)
   * @return false if a *is not* an instance of [Bool](#Bool)
   */
  static auto classof(const Ast *ast) -> bool;

  /**
   * @brief This function converts the boolean value held into it's textual
   * representation.
   *
   * @return std::string if [value](@ref Bool::value) is true then "true"
   *                     otherwise "false"
   */
  [[nodiscard]] auto ToString() const -> std::string override;

  /**
   * @brief This function converts the boolean value held into it's llvm
   * representation.
   *
   * This function will never construct an [Error](#Error)
   * it always returns a [ConstantInt] with length 1.
   * that is, an "i1", holding precisely [value](@ref Bool::value)
   *
   * [llvmValue]: https://llvm.org/doxygen/classllvm_1_1Value.html
   * "llvm::Value*" [ConstantInt]:
   * https://llvm.org/doxygen/classllvm_1_1ConstantInt.html "llvm::ConstantInt*"
   * [instruction_builder]: https://llvm.org/doxygen/classllvm_1_1IRBuilder.html
   * "instruction builder"
   *
   * @param env The Environment which holds the [instruction_builder] used to
   * construct the [ConstantInt]
   * @return Outcome<llvm::Value*, Error> an [llvmValue] holding a [ConstantInt]
   */
  [[nodiscard]] auto Codegen(const Environment &env) const
      -> Outcome<llvm::Value *, Error> override;
};
} // namespace pink
