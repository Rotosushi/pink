/**
 * @file Dot.h
 * @brief Header for class Dot
 * @version 0.1
 */
#pragma once 
#include "ast/Ast.h"


namespace pink {
  /**
   * @brief Represents a member access expression.
   * 
   * this expression represents extracting a value from an aggregate type
   * 
   */
  class Dot : public Ast {
  private:
    /**
     * @brief Compute the Type of the Dot expression
     * 
     * @param env the environment of this compilation unit
     * @return Outcome<Type*, Error> if true the type of the dot expression,
     * if false the Error encountered
     */
    virtual Outcome<Type*, Error> GetypeV(const Environment& env) override;

  public:
    /**
     * @brief the left hand side expression
     * 
     * the aggregate being accessed
     */
    std::unique_ptr<Ast> left;

    /**
     * @brief the right hand side expression
     * 
     * the index to compute
     */
    std::unique_ptr<Ast> right;

    /**
     * @brief Construct a new Dot
     * 
     * @param location the textual location of the Dot expression
     * @param left the left hand side expression
     * @param right the right hand side expression
     */
    Dot(Location location, std::unique_ptr<Ast> left, std::unique_ptr<Ast> right);

    /**
     * @brief Destroy the Dot
     * 
     */
    virtual ~Dot();

    /**
     * @brief Implements LLVM style [RTTI] for this class
     * 
     * [RTTI]: https://llvm.org/docs/HowToSetUpLLVMStyleRTTI.html "RTTI"
     * 
     * @param ast the class to test
     * @return true if ast *is* an instance of Dot
     * @return false if ast *is not* an instance of Dot
     */
    static bool classof(const Ast* ast);

    /**
     * @brief Return the cannonical string representation of this Dot expression
     * 
     * @return std::string the string representation
     */
    virtual std::string ToString() override;

    /**
     * @brief Compute the value of this Dot expression
     * 
     * @param env the environment of this compilation unit
     * @return Outcome<llvm::Value*, Error> if true the Value of the Dot expression,
     * if false the Error encountered.
     */
    virtual Outcome<llvm::Value*, Error> Codegen(const Environment& env) override;
  };
}
