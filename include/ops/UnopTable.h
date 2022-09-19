/**
 * @file UnopTable.h
 * @brief Header for class UnopTable
 * @version 0.1
 */
#pragma once
#include <utility> // std::pair
#include <memory>  // std::unique_ptr

#include "llvm/ADT/APInt.h"
#include "llvm/ADT/DenseMap.h"

#include "aux/StringInterner.h"

#include "ops/UnopLiteral.h"

namespace pink {
    /**
     * @brief Represents the table of all known unary operators
     * 
     */
    class UnopTable {
    private:
        /**
         * @brief The [table] of all known unops 
         * [table]: https://llvm.org/doxygen/DenseMap_8h.html "table"
         */ 
        llvm::DenseMap<InternedString, std::unique_ptr<UnopLiteral>> table;

    public:
        /**
         * @brief Construct a new Unop Table
         * 
         */
        UnopTable();

        /**
         * @brief Destroy the Unop Table
         * 
         */
        ~UnopTable();

        /**
         * @brief Register a new unary operator with no implementations
         * 
         * @param op the operator to register in the table
         * @return std::pair<InternedString, UnopLiteral*> the new operator
         */
        std::pair<InternedString, UnopLiteral*> Register(InternedString op);

        /**
         * @brief Register a new unary operator with one implementation
         * 
         * @param op the operator to register in the table
         * @param arg_t the argument Type of the implementation
         * @param ret_t the return Type of the implementation
         * @param fn the generator function of the implementation
         * @return std::pair<InternedString, UnopLiteral*> the new operator
         */
        std::pair<InternedString, UnopLiteral*> Register(InternedString op, Type* arg_t, Type* ret_t, UnopCodegenFn fn);

        /**
         * @brief Remove an operator from the table
         * 
         * @param op the operator to remove
         */
        void Unregister(InternedString op);

        /**
         * @brief Lookup an operator from the table
         * 
         * @param op the operator to lookup
         * @return llvm::Optional<std::pair<InternedString, UnopLiteral*>> if has_value, the operator from the table,
         * otherwise nothing.
         */
        llvm::Optional<std::pair<InternedString, UnopLiteral*>> Lookup(InternedString op);
    };
}
