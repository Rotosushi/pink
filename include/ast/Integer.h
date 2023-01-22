/**
 * @file Integer.h
 * @brief Header for class Integer
 * @version 0.1
 */
#pragma once
#include <string>

#include "ast/Ast.h"

namespace pink {
/**
 * @brief Represents an integer value within an expression
 *
 * can hold any integer from -2^64 to 2^(64) - 1
 *
 * because we choose to have Integer's size be equivalent to the word size of
 * the machine. it's an 'i64' in llvm IR.
 *
 * \todo The size of Integer is not selected dynamically based upon the target
 * machine
 *
 * \todo #CPP can be lowered to int
 *
 */
class Integer : public Ast {
private:
  /**
   * @brief returns an [IntegerType](#IntegerType) from the
   * [TypeInterner](#TypeInterner)
   *
   *  This function never constructs an [Error](#Error)
   *
   * @param env The [Environment](#Environment) to get the
   * [IntegerType](#IntegerType) from.
   * @return Outcome<Type*, Error> An pointer to an [IntegerType](#IntegerType)
   */
  [[nodiscard]] auto TypecheckV(const Environment &env) const
      -> Outcome<Type *, Error> override;

public:
  /**
   * @brief holds the integer being represented.
   */
  long long value;

  /**
   * @brief Construct a new Integer
   *
   * @param l The textual [location](#Location) of the integer
   * @param i The value of the integer
   */
  Integer(const Location &location, const long long value);

  /**
   * @brief Destroy the Integer
   *
   */
  ~Integer() override = default;

  Integer(const Integer &other) = delete;

  Integer(Integer &&other) = default;

  auto operator=(const Integer &other) -> Integer & = delete;

  auto operator=(Integer &&other) -> Integer & = default;

  /**
   * @brief Get the Value
   *
   * @return long long the value
   */
  auto GetValue() const -> long long { return value; }

  /**
   * @brief This function is used to implement llvm style [RTTI] for this node
   * kind
   *
   * [RTTI]: https://llvm.org/docs/HowToSetUpLLVMStyleRTTI.html "RTTI"
   *
   * @param ast The Ast* being tested
   * @return true if ast *is* an instance of an [Integer](#Integer)
   * @return false if ast *is not* an instance of an [Integer](#Integer)
   */
  static auto classof(const Ast *ast) -> bool;

  /**
   * @brief returns the textual representation of this integer value.
   *
   * @return std::string this is equal to std::to_string(value)
   */
  [[nodiscard]] auto ToString() const -> std::string override;

  /**
   * @brief Translates the value into a [ConstantInt] of length 64
   *
   * [ConstantInt]: https://llvm.org/doxygen/classllvm_1_1ConstantInt.html
   * "llvm::ConstantInt*"
   *
   * This function never constructs an [Error](#Error)
   *
   * @param env The [Environment](#Environment) which is used to retrieve the
   * [ConstantInt]
   * @return Outcome<llvm::Value*, Error> The [ConstantInt] holding the
   * [value](@ref Integer::value)
   */
  [[nodiscard]] auto Codegen(const Environment &env) const
      -> Outcome<llvm::Value *, Error> override;
};
} // namespace pink
