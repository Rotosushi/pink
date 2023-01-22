/**
 * @file Unop.h
 * @brief Header for class Unop
 * @version 0.1
 */
#pragma once
#include <string>

#include "ast/Ast.h"
#include "aux/StringInterner.h"

namespace pink {
/**
 * @brief Represents a unary operator expression
 *
 * \todo #CPP can be lowered to cpp unops
 */
class Unop : public Ast {
private:
  /**
   * @brief Compute the Type of this Unop expression
   *
   * @param env the environment of this compilation unit
   * @return Outcome<Type*, Error> if true the Type of this Unop expression,
   * if false the Error encountered
   */
  [[nodiscard]] auto TypecheckV(const Environment &env) const
      -> Outcome<Type *, Error> override;

public:
  /**
   * @brief the Unary Operator this expression represents
   *
   */
  InternedString op;

  /**
   * @brief The argument to the Unary operation
   *
   */
  std::unique_ptr<Ast> right;

  /**
   *
   * @brief Construct a new Unop
   *
   * @param location the textual location of this Unop expression
   * @param opr the unary operator
   * @param right the argument to the operation
   */
  Unop(const Location &loc, InternedString opr, std::unique_ptr<Ast> right);

  /**
   * @brief Destroy the Unop
   *
   */
  ~Unop() override = default;

  Unop(const Unop &other) = delete;

  Unop(Unop &&other) = default;

  auto operator=(const Unop &other) -> Unop & = delete;

  auto operator=(Unop &&other) -> Unop & = default;

  auto GetOp() const -> InternedString { return op; }

  auto GetRight() const -> const Ast * { return right.get(); }

  /**
   * @brief Implements LLVM style [RTTI] for this class
   *
   * [RTTI]: https://llvm.org/docs/HowToSetUpLLVMStyleRTTI.html "RTTI"
   *
   * @param ast the ast to test
   * @return true if ast *is* an instance of a Unop expression
   * @return false if ast *is not* an instance of a Unop expression
   */
  static auto classof(const Ast *ast) -> bool;

  /**
   * @brief Compute the cannonical string representation of this Unop expression
   *
   * @return std::string the string representation
   */
  [[nodiscard]] auto ToString() const -> std::string override;

  /**
   * @brief Compute the result Value of this Unop Expression
   *
   * @param env the environment of this compilation unit
   * @return Outcome<llvm::Value*, Error> if true the result Value of this Unop
   * expression, if false the Error encountered
   */
  [[nodiscard]] auto Codegen(const Environment &env) const
      -> Outcome<llvm::Value *, Error> override;
};
} // namespace pink
