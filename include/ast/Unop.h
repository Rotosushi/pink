/**
 * @file Unop.h
 * @brief Header for class Unop
 * @version 0.1
 */
#pragma once
#include <string>

#include "ast/Ast.h"
#include "aux/StringInterner.h"

namespace pink {
    /**
     * @brief Represents a unary operator expression
     * 
     */
    class Unop : public Ast {
    private:
        /**
         * @brief Compute the Type of this Unop expression
         * 
         * @param env the environment of this compilation unit
         * @return Outcome<Type*, Error> if true the Type of this Unop expression,
         * if false the Error encountered
         */
   		virtual Outcome<Type*, Error> GetypeV(const Environment& env) override;
    public:
        /**
         * @brief the Unary Operator this expression represents
         * 
         */
        InternedString op;

        /**
         * @brief The argument to the Unary operation
         * 
         */
        std::unique_ptr<Ast> right;

    /**
     * @brief Construct a new Unop
     * 
     * @param loc the textual location of this Unop expression 
     * @param op the unary operator
     * @param right the argument to the operation
     */
    Unop(Location& loc, InternedString op, std::unique_ptr<Ast> right);

    /**
     * @brief Destroy the Unop
     * 
     */
    virtual ~Unop();

    /**
     * @brief Implements LLVM style [RTTI] for this class
     * 
     * [RTTI]: https://llvm.org/docs/HowToSetUpLLVMStyleRTTI.html "RTTI"
     * 
     * @param ast the ast to test
     * @return true if ast *is* an instance of a Unop expression
     * @return false if ast *is not* an instance of a Unop expression
     */
    static bool classof(const Ast* ast);

    /**
     * @brief Compute the cannonical string representation of this Unop expression
     * 
     * @return std::string the string representation
     */
    virtual std::string ToString() override;
    
    /**
     * @brief Compute the result Value of this Unop Expression
     * 
     * @param env the environment of this compilation unit
     * @return Outcome<llvm::Value*, Error> if true the result Value of this Unop expression,
     * if false the Error encountered
     */
    virtual Outcome<llvm::Value*, Error> Codegen(const Environment& env) override;
    };
}
