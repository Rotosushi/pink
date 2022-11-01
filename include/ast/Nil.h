/**
 * @file Nil.h
 * @brief Header for class Nil
 * @version 0.1
 *
 */
#pragma once
#include <iostream>
#include <string>

#include "ast/Ast.h"

namespace pink {
/**
 * @brief Represents the single valued type 'nil'
 *
 * it's value is equivalent to 'false.'
 * it's useful for many of the same reasons the
 * type 'void' is useful in C/C++.
 *
 * \todo since we had to add the void type in order
 * to implement while loops, maybe we should remove
 * nil from the language.
 *
 * \todo can be lowered to bool, but it might do to simply
 * remove it.
 */
class Nil : public Ast {
private:
  /**
   * @brief Computes the [NilType](#NilType) from the
   * [Environment](#Environment)
   *
   * This function never constructs an [Error](#Error)
   *
   * @param env The [Environment](#Environment) to retrieve the
   * [NilType](#NilType) from
   * @return Outcome<Type*, Error> An instance of [NilType](#NilType)
   */
  [[nodiscard]] auto TypecheckV(const Environment &env) const
      -> Outcome<Type *, Error> override;

public:
  /**
   * @brief Construct a new Nil
   *
   * since there is only the single value being represented,
   * we always know what it is in advance, and don't need to store
   * anything.
   *
   * @param l The textual [location](#Location) of the nil
   */
  Nil(const Location &location);

  /**
   * @brief Destroy the Nil
   *
   */
  ~Nil() override = default;

  Nil(const Nil &other) = delete;

  Nil(Nil &&other) = default;

  auto operator=(const Nil &other) -> Nil & = delete;

  auto operator=(Nil &&other) -> Nil & = default;

  /**
   * @brief part of the Visitor interface
   *
   * @param visitor the visitor to accept
   */
  void Accept(AstVisitor *visitor) const override;

  /**
   * @brief This function is used to implement llvm style [RTTI] for this node
   * kind
   *
   * [RTTI]: https://llvm.org/docs/HowToSetUpLLVMStyleRTTI.html "RTTI"
   *
   * @param ast The Ast* being tested
   * @return true if ast *is* an instance of an [Nil](#Nil)
   * @return false if ast *is not* an instance of an [Nil](#Nil)
   */
  static auto classof(const Ast *ast) -> bool;

  /**
   * @brief returns the textual representation of a [Nil](#Nil)
   *
   * @return std::string always equal to "nil"
   */
  [[nodiscard]] auto ToString() const -> std::string override;

  /**
   * @brief Translates 'nil' into a [ConstantInt] of length 1 with value 0
   *
   * [ConstantInt]: https://llvm.org/doxygen/classllvm_1_1ConstantInt.html
   * "llvm::ConstantInt*"
   *
   * @param env The [Environment](#Environment) to retrieve the [ConstantInt]
   * from
   * @return Outcome<llvm::Value*, Error> An instance of a [ConstantInt] with
   * length 1 and value 0
   */
  [[nodiscard]] auto Codegen(const Environment &env) const
      -> Outcome<llvm::Value *, Error> override;
};
} // namespace pink
