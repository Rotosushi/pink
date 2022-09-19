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
     * @return Outcome<Type*, Error> if the types match, then the return type of the function being called,
     * if false the Error which was encountered.
     */
    virtual Outcome<Type*, Error> GetypeV(const Environment& env) override;
  public:

    /**
     * @brief Construct a new Application
     * 
     * @param location the textual location of the Application
     * @param callee the expression representing the function to be applied.
     * @param arguments the expressions representing the arguments to the function.
     */
    Application(Location location, std::unique_ptr<Ast> callee, std::vector<std::unique_ptr<Ast>> arguments);
    
    /**
     * @brief Destroy the Application
     * 
     */
    ~Application();
  
    /**
     * @brief Implements LLVM style [RTTI] for this class
     * 
     * [RTTI]: https://llvm.org/docs/HowToSetUpLLVMStyleRTTI.html "RTTI"
     * 
     * @param ast the Ast to check
     * @return true if ast *is* an instance of an Application
     * @return false if ast *is not* an instance of an Application
     */
    static bool classof(const Ast* ast);

    /**
     * @brief Compute the cannonical string representation of this Application expression
     * 
     * @return std::string the string representation
     */
    virtual std::string ToString() override;

    /**
     * @brief Compute the Value of this Application expression.
     * 
     * The value of the Application is the return value from the 
     * function after it has been called.
     * 
     * @param env the environment of this compilation unit
     * @return Outcome<llvm::Value*, Error> true if we can apply the function, and then this holds the return value,
     * or false if we cannot apply the function, and then this holds the Error encountered.
     */
    virtual Outcome<llvm::Value*, Error> Codegen(const Environment& env) override;
  };
}
