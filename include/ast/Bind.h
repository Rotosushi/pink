/**
 * @file Bind.h
 * @brief Header for class Bind
 * @version 0.1
 */
#pragma once

#include "ast/Ast.h"
#include "aux/StringInterner.h"

namespace pink {
/**
 * @brief Represents a Bind expression
 *
 */
class Bind : public Ast {
private:
  /**
   * @brief Compute the Type of the Bind expression
   *
   *  "var" [symbol] ":=" [affix] ";"
   *
   * The type of a bind expression is the type of the
   * affix expression if and only if the symbol is not
   * already bound locally within the environment.
   * and the affix expression is itself typeable.
   *
   * @param env the environment of this compilation unit
   * @return Outcome<Type*, Error> if true the type of the Bind expression,
   * if false the Error encountered.
   */
  [[nodiscard]] auto GetypeV(const Environment &env) const
      -> Outcome<Type *, Error> override;

public:
  /**
   * @brief the symbol being bound
   *
   */
  InternedString symbol;

  /**
   * @brief the term holding the value to bind the symbol to
   *
   */
  std::unique_ptr<Ast> affix;

  /**
   * @brief Construct a new Bind
   *
   * @param location the textual location of the Bind expression
   * @param symbol the symbol to be bound
   * @param affix the expression holding the value to bind the symbol to
   */
  Bind(const Location &location, InternedString symbol,
       std::unique_ptr<Ast> affix);

  /**
   * @brief Destroy the Bind
   *
   */
  ~Bind() override = default;

  /**
   * @brief Implements LLVM style [RTTI] for this class
   *
   * [RTTI]: https://llvm.org/docs/HowToSetUpLLVMStyleRTTI.html "RTTI"
   *
   * @param ast the ast to test
   * @return true if the ast *is* an instance of a Bind expression
   * @return false if the ast *is not* an instance of a Bind expression
   */
  static auto classof(const Ast *ast) -> bool;

  /**
   * @brief Compute the cannonical string representation of the Bind expression
   *
   * @return std::string the string representation
   */
  [[nodiscard]] auto ToString() const -> std::string override;

  /**
   * @brief Compute the Value of the Bind expression
   *
   * "var" [symbol] ":=" [affix] ";"
   *
   * The value of a bind expression is the value of
   * the affix expression if and only if the symbol
   * has not been locally bound. and the affix expression
   * itself has a computable value.
   *
   * @param env the environment of the compilation unit
   * @return Outcome<llvm::Value*, Error> if true the value of the Bind
   * expression, if false the Error encountered
   */
  [[nodiscard]] auto Codegen(const Environment &env) const
      -> Outcome<llvm::Value *, Error> override;
};
} // namespace pink
