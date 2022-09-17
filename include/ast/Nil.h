/**
 * @file Nil.h
 * @brief Header for class Nil
 * @version 0.1
 * 
 */
#pragma once
#include <string>
#include <iostream>

#include "ast/Ast.h"

namespace pink {
    /**
     * @brief Represents the single valued type 'nil'
     * 
     * it's value is equivalent to 'false.'
     * it's useful for many of the same reasons the 
     * type 'void' is useful in C/C++.
     */
    class Nil : public Ast {
    private:
        /**
         * @brief Computes the [NilType](#NilType) from the [Environment](#Environment)
         * 
         * This function never constructs an [Error](#Error)
         * 
         * @param env The [Environment](#Environment) to retrieve the [NilType](#NilType) from
         * @return Outcome<Type*, Error> An instance of [NilType](#NilType)
         */
    	virtual Outcome<Type*, Error> GetypeV(const Environment& env) override;
    
    public:
        /**
         * @brief Construct a new Nil
         * 
         * since there is only the single value being represented,
         * we always know what it is in advance, and don't need to store 
         * anything.
         * 
         * @param l The textual [location](#Location) of the nil
         */ 
        Nil(Location l);

        /**
         * @brief Destroy the Nil
         * 
         */
        virtual ~Nil() override;

        /**
         * @brief This function is used to implement llvm style [RTTI] for this node kind
         * 
         * [RTTI]: https://llvm.org/docs/HowToSetUpLLVMStyleRTTI.html "RTTI"
         * 
         * @param ast The Ast* being tested 
         * @return true if ast *is* an instance of an [Nil](#Nil)
         * @return false if ast *is not* an instance of an [Nil](#Nil)
         */
        static bool classof(const Ast* ast);

        /**
         * @brief returns the textual representation of a [Nil](#Nil)
         * 
         * @return std::string always equal to "nil"
         */
        virtual std::string ToString() override;

        /**
         * @brief Translates 'nil' into a [ConstantInt] of length 1 with value 0
         * 
         * [ConstantInt]: https://llvm.org/doxygen/classllvm_1_1ConstantInt.html "llvm::ConstantInt*" 
         * 
         * @param env The [Environment](#Environment) to retrieve the [ConstantInt] from
         * @return Outcome<llvm::Value*, Error> An instance of a [ConstantInt] with length 1 and value 0
         */
        virtual Outcome<llvm::Value*, Error> Codegen(const Environment& env) override;
    };
}
