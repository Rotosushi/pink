/**
 * @file Boolean.h
 * @brief Header for class Boolean
 * @version 0.1
 *
 */
#pragma once
#include <string>

#include "ast/Ast.h"

namespace pink {
/**
 * @brief [Boolean](#Boolean) is an [Ast](#Ast) node which represents a boolean
 * value
 *
 *
 * @headerfile Boolean.h "ast/Boolean.h"
 */
class Boolean : public Ast {
private:
  /**
   * @brief returns [BooleanType](#BooleanType)
   *
   * This function never constructs an [Error](#Error)
   *
   * @param env The [Environment](#Environment) which is used to get a
   * [BooleanType](#BooleanType) from the [TypeInterner](#TypeInterner)
   * @return Outcome<Type*, Error> A [BooleanType](#BooleanType) from the
   * [TypeInterner](#TypeInterner) held in [env](#Environment)
   */
  [[nodiscard]] auto TypecheckV(const Environment &env) const
      -> Outcome<Type *, Error> override;

public:
  /**
   * @brief represents the state of this boolean value
   *
   * can be either true, or false.
   */
  bool value;

  /**
   * @brief Construct a new Boolean
   *
   * @param location The textual [location] of the bool
   * @param value The value of the bool
   */
  Boolean(const Location &location, const bool value);

  /**
   * @brief Destroy the Boolean
   */
  ~Boolean() override = default;

  Boolean(const Boolean &other) = delete;

  Boolean(Boolean &&other) = default;

  auto operator=(const Boolean &other) -> Boolean & = delete;

  auto operator=(Boolean &&other) -> Boolean & = default;

  /**
   * @brief part of the Visitor interface
   *
   * @param visitor the visitor to accept
   */
  void Accept(const ConstAstVisitor *visitor) const override;

  /**
   * @brief This function is used to implement llvm style [RTTI] for this node
   * kind
   *
   * [RTTI]: https://llvm.org/docs/HowToSetUpLLVMStyleRTTI.html "RTTI"
   *
   * @param a The Ast* being tested
   * @return true if a *is* an instance of [Boolean](#Boolean)
   * @return false if a *is not* an instance of [Boolean](#Boolean)
   */
  static auto classof(const Ast *ast) -> bool;

  /**
   * @brief Get the Value
   *
   * @return bool the value
   */
  auto GetValue() const -> bool { return value; }

  /**
   * @brief This function converts the boolean value held into it's textual
   * representation.
   *
   * @return std::string if [value](@ref Boolean::value) is true then "true"
   *                     otherwise "false"
   */
  [[nodiscard]] auto ToString() const -> std::string override;

  /**
   * @brief This function converts the boolean value held into it's llvm
   * representation.
   *
   * This function will never construct an [Error](#Error)
   * it always returns a [ConstantInt] with length 1.
   * that is, an "i1", holding precisely [value](@ref Boolean::value)
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
