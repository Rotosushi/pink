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
   */
  class Conditional : public Ast {
  private:
    /**
     * @brief Compute the Type of this conditional expression
     * 
     * @param env the environment of this compilation unit
     * @return Outcome<Type*, Error> if true the Type of this conditional expression
     * if false the Error encountered.
     */
    virtual Outcome<Type*, Error> GetypeV(const Environment& env) override;

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
     * @param loc the textual location of the conditional
     * @param test the test expression of the conditional
     * @param first the first alternative of the conditional
     * @param second the second alternative of the conditional
     */
    Conditional(Location loc, std::unique_ptr<Ast> test, std::unique_ptr<Ast> first, std::unique_ptr<Ast> second);
    
    /**
     * @brief Destroy the Conditional
     * 
     */
    ~Conditional();

    /**
     * @brief Implements LLVM style [RTTI] for this class
     * 
     * [RTTI]: https://llvm.org/docs/HowToSetUpLLVMStyleRTTI.html "RTTI"
     * 
     * @param ast the ast to test
     * @return true if ast *is* an instance of a Conditional
     * @return false if ast *is not* an instance of a Conditional
     */
    static bool classof(const Ast* ast);

    /**
     * @brief Compute the cannonical string representation of this Conditional expression
     * 
     * @return std::string the string representation
     */
    virtual std::string ToString() override;

    /**
     * @brief Compute the Value of the Conditional
     * 
     * the result value of a conditional is the result value of 
     * whichever alternative expression was evaluated.
     * 
     * @param env the environment of this compilation unit
     * @return Outcome<llvm::Value*, Error> if true then the result value of the conditional,
     * if false then the Error encountered
     */
    virtual Outcome<llvm::Value*, Error> Codegen(const Environment& env) override;
  };
}
