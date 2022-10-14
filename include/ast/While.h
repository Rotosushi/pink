/**
 * @file While.h
 * @brief Header for class While
 * @version 0.1
 */
#pragma once

#include "ast/Ast.h"

namespace pink {
/**
 * @brief Represents an instance of a While expression
 *
 */
class While : public Ast {
private:
  /**
   * @brief Compute the Type of this While expression
   *
   * @param env the environment of this compilation unit
   * @return Outcome<Type*, Error> if true the Type of this While expression,
   * if false the Error encountered
   */
  [[nodiscard]] auto TypecheckV(const Environment &env) const
      -> Outcome<Type *, Error> override;

  /**
   * @brief The test expression of the While
   *
   */
  std::unique_ptr<Ast> test;

  /**
   * @brief The body expression of the While
   *
   */
  std::unique_ptr<Ast> body;

public:
  /**
   * @brief Construct a new While
   *
   * @param location the textual location of the While expression
   * @param test the test expression of the While expression
   * @param body the body expression of the While expression
   */
  While(const Location &location, std::unique_ptr<Ast> test,
        std::unique_ptr<Ast> body);

  /**
   * @brief Destroy the While
   *
   */
  ~While() override = default;

  While(const While &other) = delete;

  While(While &&other) = default;

  auto operator=(const While &other) -> While & = delete;

  auto operator=(While &&other) -> While & = default;

  auto GetTest() const -> const Ast * { return test.get(); }

  auto GetBody() const -> const Ast * { return body.get(); }

  /**
   * @brief part of the Visitor interface
   *
   * @param visitor the visitor to accept
   */
  void Accept(AstVisitor *visitor) override;

  /**
   * @brief Compute the cannonical string representation of this While
   * expression
   *
   * @return std::string the string representation
   */
  [[nodiscard]] auto ToString() const -> std::string override;

  /**
   * @brief Implements LLVM style [RTTI] for this class
   *
   * [RTTI]: https://llvm.org/docs/HowToSetUpLLVMStyleRTTI.html "RTTI"
   *
   * @param ast the ast to test
   * @return true if ast *is* an instance of While
   * @return false if ast *is not* an instance of While
   */
  static auto classof(const Ast *ast) -> bool;

  /**
   * @brief Compute the Value of the While expression
   *
   * @param env the environment of this compilation unit
   * @return Outcome<llvm::Value*, Error> if true the Value of this While
   * expression, if false the Error encountered
   */
  [[nodiscard]] auto Codegen(const Environment &env) const
      -> Outcome<llvm::Value *, Error> override;
};
} // namespace pink
