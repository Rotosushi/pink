/**
 * @file Int.h
 * @brief Header for class Int
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
 * because we choose to have Int's size be equivalent to the word size of the
 * machine. it's an 'i64' in llvm IR.
 *
 * \todo The size of Int is not selected dynamically based upon the target
 * machine
 *
 */
class Int : public Ast {
private:
  /**
   * @brief returns an [IntType](#IntType) from the
   * [TypeInterner](#TypeInterner)
   *
   *  This function never constructs an [Error](#Error)
   *
   * @param env The [Environment](#Environment) to get the [IntType](#IntType)
   * from.
   * @return Outcome<Type*, Error> An pointer to an [IntType](#IntType)
   */
  [[nodiscard]] auto GetypeV(const Environment &env) const
      -> Outcome<Type *, Error> override;

public:
  /**
   * @brief holds the integer being represented.
   */
  long long value;

  /**
   * @brief Construct a new Int
   *
   * @param l The textual [location](#Location) of the integer
   * @param i The value of the integer
   */
  Int(const Location &location, const long long value);

  /**
   * @brief Destroy the Int
   *
   */
  ~Int() override = default;

  Int(const Int &other) = delete;

  Int(Int &&other) = default;

  auto operator=(const Int &other) -> Int & = delete;

  auto operator=(Int &&other) -> Int & = default;

  /**
   * @brief This function is used to implement llvm style [RTTI] for this node
   * kind
   *
   * [RTTI]: https://llvm.org/docs/HowToSetUpLLVMStyleRTTI.html "RTTI"
   *
   * @param ast The Ast* being tested
   * @return true if ast *is* an instance of an [Int](#Int)
   * @return false if ast *is not* an instance of an [Int](#Int)
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
   * [value](@ref Int::value)
   */
  [[nodiscard]] auto Codegen(const Environment &env) const
      -> Outcome<llvm::Value *, Error> override;
};
} // namespace pink
