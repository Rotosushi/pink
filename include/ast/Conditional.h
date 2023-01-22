/**
 * @file Conditional.h
 * @brief Header for class Conditional
 * @version 0.1
 */
#pragma once

#include "ast/Ast.h"

namespace pink {
/**
 * @brief Represents an instance of a conditional expression
 *
 * \todo #CPP lowered to conditionals
 *
 */
class Conditional : public Ast {
private:
  /**
   * @brief Compute the Type of this conditional expression
   *
   * @param env the environment of this compilation unit
   * @return Outcome<Type*, Error> if true the Type of this conditional
   * expression if false the Error encountered.
   */
  [[nodiscard]] auto TypecheckV(const Environment &env) const
      -> Outcome<Type *, Error> override;

public:
  /**
   * @brief The test statement of the conditional
   *
   */
  std::unique_ptr<Ast> test;

  /**
   * @brief The first alternative of the conditional
   *
   */
  std::unique_ptr<Ast> first;

  /**
   * @brief the second alternative of the conditional
   *
   */
  std::unique_ptr<Ast> second;

  /**
   * @brief Construct a new Conditional
   *
   * @param location the textual location of the conditional
   * @param test the test expression of the conditional
   * @param first the first alternative of the conditional
   * @param second the second alternative of the conditional
   */
  Conditional(const Location &location, std::unique_ptr<Ast> test,
              std::unique_ptr<Ast> first, std::unique_ptr<Ast> second);

  /**
   * @brief Destroy the Conditional
   *
   */
  ~Conditional() override = default;

  Conditional(const Conditional &other) = delete;

  Conditional(Conditional &&other) = default;

  auto operator=(const Conditional &other) -> Conditional & = delete;

  auto operator=(Conditional &&other) -> Conditional & = default;

  auto GetTest() const -> const Ast * { return test.get(); }

  auto GetFirst() const -> const Ast * { return first.get(); }

  auto GetSecond() const -> const Ast * { return second.get(); }

  /**
   * @brief Implements LLVM style [RTTI] for this class
   *
   * [RTTI]: https://llvm.org/docs/HowToSetUpLLVMStyleRTTI.html "RTTI"
   *
   * @param ast the ast to test
   * @return true if ast *is* an instance of a Conditional
   * @return false if ast *is not* an instance of a Conditional
   */
  static auto classof(const Ast *ast) -> bool;

  /**
   * @brief Compute the cannonical string representation of this Conditional
   * expression
   *
   * @return std::string the string representation
   */
  [[nodiscard]] auto ToString() const -> std::string override;

  /**
   * @brief Compute the Value of the Conditional
   *
   * the result value of a conditional is the result value of
   * whichever alternative expression was evaluated.
   *
   * @param env the environment of this compilation unit
   * @return Outcome<llvm::Value*, Error> if true then the result value of the
   * conditional, if false then the Error encountered
   */
  [[nodiscard]] auto Codegen(const Environment &env) const
      -> Outcome<llvm::Value *, Error> override;
};
} // namespace pink
