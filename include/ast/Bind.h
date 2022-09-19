/**
 * @file Bind.h
 * @brief Header for class Bind
 * @version 0.1
 */
#pragma once

#include "aux/StringInterner.h"
#include "ast/Ast.h"

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
         * @param env the environment of this compilation unit
         * @return Outcome<Type*, Error> if true the type of the Bind expression,
         * if false the Error encountered.
         */
    	virtual Outcome<Type*, Error> GetypeV(const Environment& env) override;
    
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
        std::unique_ptr<Ast> term;

        /**
         * @brief Construct a new Bind
         * 
         * @param loc the textual location of the Bind expression
         * @param symbol the symbol to be bound
         * @param term the expression holding the value to bind the symbol to
         */
        Bind(Location loc, InternedString symbol, std::unique_ptr<Ast> term);

        /**
         * @brief Destroy the Bind 
         * 
         */
        virtual ~Bind();

        /**
         * @brief Implements LLVM style [RTTI] for this class
         * 
         * [RTTI]: https://llvm.org/docs/HowToSetUpLLVMStyleRTTI.html "RTTI"
         * 
         * @param ast the ast to test
         * @return true if the ast *is* an instance of a Bind expression
         * @return false if the ast *is not* an instance of a Bind expression
         */
        static bool classof(const Ast* ast);

        /**
         * @brief Compute the cannonical string representation of the Bind expression
         * 
         * @return std::string the string representation
         */
        virtual std::string ToString() override;
        
        /**
         * @brief Compute the Value of the Bind expression
         * 
         * returns the Value of the term representing the Bind expression
         * 
         * @param env the environment of the compilation unit
         * @return Outcome<llvm::Value*, Error> if true the value of the Bind expression,
         * if false the Error encountered
         */
        virtual Outcome<llvm::Value*, Error> Codegen(const Environment& env) override;
    };
}
