/**
 * @file BinopLiteral.h
 * @brief Header for class BinopLiteral
 * @version 0.1
 */
#pragma once
#include <utility> // std::pair
#include <memory>  // std::unique_ptr

#include "llvm/ADT/APInt.h"
#include "llvm/ADT/DenseMap.h"

#include "ops/BinopCodegen.h"
#include "ops/PrecedenceAndAssociativity.h"

namespace pink {
    /**
     * @brief Represents a single binary operator
     * 
     */
    class BinopLiteral {
    private:
        /**
         * @brief the set of generator functions associated with this binop
         * 
         */
        llvm::DenseMap<std::pair<Type*, Type*>, std::unique_ptr<BinopCodegen>> overloads;

    public:
        /**
         * @brief The precedence of this binary operator
         * 
         */
        Precedence precedence;

        /**
         * @brief The associativity of this binary operator
         * 
         */
        Associativity associativity;


        BinopLiteral() = delete;
        /**
         * @brief Construct a new Binop Literal
         * 
         * @param p the precedence of this binop
         * @param a the associativity of this binop
         */
        BinopLiteral(Precedence p, Associativity a);

        /**
         * @brief Construct a new Binop Literal
         * 
         * @param p the precedence of this binop
         * @param a the associativity of this binop
         * @param left_t the left argument Type of the generator function
         * @param right_t the right argument Type of the generator function
         * @param ret_t the return Type of the generator function
         * @param fn the generator function
         */
        BinopLiteral(Precedence p, Associativity a, Type* left_t, Type* right_t, Type* ret_t, BinopCodegenFn fn);

        /**
         * @brief Destroy the Binop Literal
         * 
         */
        ~BinopLiteral();

        /**
         * @brief Return the number of implementations this binop has
         * 
         * @return unsigned the number of overloads of this binop
         */
        unsigned NumOverloads() const;

        /**
         * @brief Register a new implementation of this binop
         * 
         * @param left_t the left argument Type of the generator function
         * @param right_t the right argument Type of the generator function
         * @param ret_t the return Type of the generator function
         * @param fn the generator function
         * @return std::pair<std::pair<Type*, Type*>, BinopCodegen*> the new overload
         */
        std::pair<std::pair<Type*, Type*>, BinopCodegen*> Register(Type* left_t, Type* right_t, Type* ret_t, BinopCodegenFn fn);

        /**
         * @brief Remove an existing implementation of this binop
         * 
         * @param left_t the left argument Type of the implementation to remove
         * @param right_t the right argument Type of the implementation to remove
         */
        void Unregister(Type* left_t, Type* right_t);

        /**
         * @brief Lookup an implementation of the binop corresponding to the given Types
         * 
         * @param left_t the left argument Type of the implementation
         * @param right_t the right argument Type of the implementation
         * @return llvm::Optional<std::pair<std::pair<Type*, Type*>, BinopCodegen*>> if true the implementation of the binop,
         * if false then nothing.
         */
        llvm::Optional<std::pair<std::pair<Type*, Type*>, BinopCodegen*>> Lookup(Type* left_t, Type* right_t);
    };
}
