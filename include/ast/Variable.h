/**
 * @file Variable.h
 * @brief Header for class Variable
 * @version 0.1
 */
#pragma once
#include <string>

#include "ast/Ast.h"
#include "aux/StringInterner.h"

namespace pink {
/**
 * @brief Represents a Variable expression
 *
 */
class Variable : public Ast {
private:
  /**
   * @brief Compute the Type of this Unop expression
   *
   * @param env the environment of this compilation unit
   * @return Outcome<Type*, Error> if true, the Type of the Unop expression,
   * if false the Error encountered
   */
  [[nodiscard]] auto TypecheckV(const Environment &env) const
      -> Outcome<Type *, Error> override;

  /**
   * @brief The symbol which this Variable expression represents
   *
   */
  InternedString symbol;

public:
  /**
   * @brief Construct a new Variable
   *
   * @param loc the textual location of this Variable
   * @param symbol the symbol this Variable represents
   */
  Variable(const Location &loc, InternedString symbol);

  /**
   * @brief Destroy the Variable
   *
   */
  ~Variable() override = default;

  Variable(const Variable &other) = delete;

  Variable(Variable &&other) = default;

  auto operator=(const Variable &other) -> Variable & = delete;

  auto operator=(Variable &&other) -> Variable & = default;

  auto GetSymbol() const -> InternedString { return symbol; }

  /**
   * @brief part of the Visitor interface
   *
   * @param visitor the visitor to accept
   */
  void Accept(AstVisitor *visitor) const override;

  /**
   * @brief Implements LLVM style [RTTI] for this class
   *
   * [RTTI]: https://llvm.org/docs/HowToSetUpLLVMStyleRTTI.html "RTTI"
   *
   * @param ast the ast to test
   * @return true if ast *is* an instance of Variable
   * @return false if ast *is not* an instance of Variable
   */
  static auto classof(const Ast *ast) -> bool;

  /**
   * @brief Computes the cannonical string representation of this Variable
   * expression
   *
   * @return std::string the string representation
   */
  [[nodiscard]] auto ToString() const -> std::string override;

  /**
   * @brief Computes the Value of this Variable expression
   *
   * @param env the environment of this compilation unit
   * @return Outcome<llvm::Value*, Error> if true the Value of this Variable
   * expression, if false the Error encountered
   */
  [[nodiscard]] auto Codegen(const Environment &env) const
      -> Outcome<llvm::Value *, Error> override;
};
} // namespace pink
