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
    virtual Outcome<Type*, Error> GetypeV(const Environment& env) override;

  public:
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

    /**
     * @brief Construct a new While
     * 
     * @param loc the textual location of the While expression
     * @param test the test expression of the While expression
     * @param body the body expression of the While expression
     */
    While(Location loc, std::unique_ptr<Ast> test, std::unique_ptr<Ast> body);

    /**
     * @brief Destroy the While
     * 
     */
    virtual ~While();

    /**
     * @brief Compute the cannonical string representation of this While expression
     * 
     * @return std::string the string representation
     */
    virtual std::string ToString() override;

    /**
     * @brief Implements LLVM style [RTTI] for this class
     * 
     * [RTTI]: https://llvm.org/docs/HowToSetUpLLVMStyleRTTI.html "RTTI"
     * 
     * @param ast the ast to test
     * @return true if ast *is* an instance of While
     * @return false if ast *is not* an instance of While
     */
    static bool classof(const Ast* ast);

    /**
     * @brief Compute the Value of the While expression
     * 
     * @param env the environment of this compilation unit
     * @return Outcome<llvm::Value*, Error> if true the Value of this While expression,
     * if false the Error encountered
     */
    virtual Outcome<llvm::Value*, Error> Codegen(const Environment& env) override;
  };
}
