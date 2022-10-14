/**
 * @file Assignment.h
 * @brief Header for class Assignment
 * @version 0.1
 */
#pragma once
#include "ast/Ast.h"

namespace pink {
/**
 * @brief Represents an Assignment expression
 *
 */
class Assignment : public Ast {
private:
  /**
   * @brief Compute the Type of this Assignment expression
   *
   * @param env the environment of this compilation unit
   * @return Outcome<Type*, Error> if true the Type of the Assignment
   * expression, if false the Error encountered.
   */
  [[nodiscard]] auto TypecheckV(const Environment &env) const
      -> Outcome<Type *, Error> override;

  /**
   * @brief the left hand side of the assignment, the destination
   *
   */
  std::unique_ptr<Ast> left;

  /**
   * @brief the right hand side of the assignment, the source
   *
   */
  std::unique_ptr<Ast> right;

public:
  /**
   * @brief Construct a new Assignment
   *
   * @param loc the textual location of this assignment
   * @param left the left hand side expression
   * @param right the right hand side expression
   */
  Assignment(const Location &location, std::unique_ptr<Ast> left,
             std::unique_ptr<Ast> right);

  /**
   * @brief Destroy the Assignment
   *
   */
  ~Assignment() override = default;

  Assignment(const Assignment &other) = delete;

  Assignment(Assignment &&other) = default;

  auto operator=(const Assignment &other) -> Assignment & = delete;

  auto operator=(Assignment &&other) -> Assignment & = default;

  auto GetLeft() const -> Ast * { return left.get(); }

  auto GetRight() const -> Ast * { return right.get(); }

  /**
   * @brief part of the Visitor interface
   *
   * @param visitor the visitor to accept
   */
  void Accept(AstVisitor *visitor) override;

  /**
   * @brief Implements LLVM style [RTTI] for this class
   *
   * [RTTI]: https://llvm.org/docs/HowToSetUpLLVMStyleRTTI.html "RTTI"
   *
   * @param ast the ast to test
   * @return true if ast *is* an instance of an Assignment
   * @return false if ast *is not* an instance of an Assignment
   */
  static auto classof(const Ast *ast) -> bool;

  /**
   * @brief Compute the cannonical string representation of this Assignment
   * expression
   *
   * @return std::string the string representation
   */
  [[nodiscard]] auto ToString() const -> std::string override;

  /**
   * @brief Compute the Value of the Assignment expression
   *
   *  returns the value received from the right hand side expression
   *
   * @param env the environment of this compilation unit
   * @return Outcome<llvm::Value*, Error> if true the value of the assignment
   * expression, if false the Error encountered.
   */
  [[nodiscard]] auto Codegen(const Environment &env) const
      -> Outcome<llvm::Value *, Error> override;
};
} // namespace pink
