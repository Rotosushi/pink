/**
 * @file UnopLiteral.h
 * @brief Header for class UnopLiteral
 * @version 0.1
 */
#pragma once
#include <utility> // std::pair
#include <memory>  // std::shared_ptr

#include "llvm/ADT/APInt.h"
#include "llvm/ADT/DenseMap.h"

#include "ops/UnopCodegen.h"

namespace pink {
    /**
     * @brief Represents a single unary operator
     * 
     */
    class UnopLiteral {
    private:
        /**
         * @brief a [table] of all implementations of this unop
         * 
         * [table]: https://llvm.org/doxygen/DenseMap_8h.html "table"
         * 
         */
        
        llvm::DenseMap<Type*, std::unique_ptr<UnopCodegen>> overloads;

    public:
        /**
         * @brief Construct a new Unop Literal, with no implementations
         * 
         */
        UnopLiteral();

        /**
         * @brief Construct a new Unop Literal, with one implementation
         * 
         * @param arg_t the argument Type for this unop
         * @param ret_t the return Type for this unop
         * @param fn the generator function for this unop
         */
        UnopLiteral(Type* arg_t, Type* ret_t, UnopCodegenFn fn);

        /**
         * @brief Destroy the Unop Literal 
         * 
         */
        ~UnopLiteral();

        /**
         * @brief Register a new implementation of this Unop
         * 
         * @param arg_t the argument Type of the new implementation
         * @param ret_t the return Type of the new implementation
         * @param fn the generator function for the new implementation
         * @return std::pair<Type*, UnopCodegen*> The new implementation
         */
        std::pair<Type*, UnopCodegen*> Register(Type* arg_t, Type* ret_t, UnopCodegenFn fn);

        /**
         * @brief Remove an implementation of this unop
         * 
         * @param a the argument type of the implementation to remove
         */
        void Unregister(Type* a);

        /**
         * @brief Lookup an implementation of this unop for the given argument Type.
         * 
         * @param arg_t the argument Type to lookup
         * @return llvm::Optional<std::pair<Type*, UnopCodegen*>> if has_value the implementation for the given Type,
         * otherwise nothing. 
         */
        llvm::Optional<std::pair<Type*, UnopCodegen*>> Lookup(Type* arg_t);
    };
}
