/**
 * @file Application.h
 * @brief Header for class Application
 * @version 0.1
 */
#pragma once
#include <vector>

#include "ast/Ast.h"

namespace pink {
/**
 * @brief Represents an expression applying a function.
 *
 */
class Application : public Ast {
private:
  /**
   * @brief The function to call
   *
   */
  std::unique_ptr<Ast> callee;

  /**
   * @brief The arguments to pass
   *
   */
  std::vector<std::unique_ptr<Ast>> arguments;

  /**
   * @brief Compute the Type of this Application expression
   *
   * If all the given argument types match the functions formal argument
   * types then the type of the application term is the return type of
   * the function called.
   *
   * @param env The environment of this compilation unit.
   * @return Outcome<Type*, Error> if the types match, then the return type of
   * the function being called, if false the Error which was encountered.
   */
  [[nodiscard]] auto GetypeV(const Environment &env) const
      -> Outcome<Type *, Error> override;

public:
  /**
   * @brief Construct a new Application
   *
   * @param location the textual location of the Application
   * @param callee the expression representing the function to be applied.
   * @param arguments the expressions representing the arguments to the
   * function.
   */
  Application(const Location &location, std::unique_ptr<Ast> callee,
              std::vector<std::unique_ptr<Ast>> arguments);

  /**
   * @brief Destroy the Application
   *
   */
  ~Application() override = default;

  Application(const Application &other) = delete;

  Application(Application &&other) = default;

  auto operator=(const Application &other) -> Application & = delete;

  auto operator=(Application &&other) -> Application & = default;

  /**
   * @brief Implements LLVM style [RTTI] for this class
   *
   * [RTTI]: https://llvm.org/docs/HowToSetUpLLVMStyleRTTI.html "RTTI"
   *
   * @param ast the Ast to check
   * @return true if ast *is* an instance of an Application
   * @return false if ast *is not* an instance of an Application
   */
  static auto classof(const Ast *ast) -> bool;

  /**
   * @brief Compute the cannonical string representation of this Application
   * expression
   *
   * @return std::string the string representation
   */
  [[nodiscard]] auto ToString() const -> std::string override;

  /**
   * @brief Compute the Value of this Application expression.
   *
   * The value of the Application is the return value from the
   * function after it has been called.
   *
   * @param env the environment of this compilation unit
   * @return Outcome<llvm::Value*, Error> true if we can apply the function, and
   * then this holds the return value, or false if we cannot apply the function,
   * and then this holds the Error encountered.
   */
  [[nodiscard]] auto Codegen(const Environment &env) const
      -> Outcome<llvm::Value *, Error> override;
};
} // namespace pink
