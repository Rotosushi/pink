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
    	virtual Outcome<Type*, Error> GetypeV(const Environment& env) override;
    	
    public:
        /**
         * @brief The symbol which this Variable expression represents
         * 
         */
        InternedString symbol;

        /**
         * @brief Construct a new Variable
         * 
         * @param loc the textual location of this Variable
         * @param symbol the symbol this Variable represents
         */
        Variable(Location loc, InternedString symbol);

        /**
         * @brief Destroy the Variable
         * 
         */
        virtual ~Variable();

        /**
         * @brief Implements LLVM style [RTTI] for this class
         * 
         * [RTTI]: https://llvm.org/docs/HowToSetUpLLVMStyleRTTI.html "RTTI"
         * 
         * @param ast the ast to test
         * @return true if ast *is* an instance of Variable
         * @return false if ast *is not* an instance of Variable
         */
        static bool classof(const Ast* ast);

        /**
         * @brief Computes the cannonical string representation of this Variable expression
         * 
         * @return std::string the string representation
         */
        virtual std::string ToString() override;

        /**
         * @brief Computes the Value of this Variable expression
         * 
         * @param env the environment of this compilation unit
         * @return Outcome<llvm::Value*, Error> if true the Value of this Variable expression,
         * if false the Error encountered
         */
        virtual Outcome<llvm::Value*, Error> Codegen(const Environment& env) override;
    };
}
