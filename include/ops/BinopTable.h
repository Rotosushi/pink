/**
 * @file BinopTable.h
 * @brief Header for class BinopTable
 * @version 0.1
 */
#pragma once
#include <utility> // std::pair<>
#include <memory>  // std::unique_ptr

#include "llvm/ADT/APInt.h" // llvm::Optional<>
#include "llvm/ADT/DenseMap.h" // llvm::DenseMap<>

#include "aux/StringInterner.h"

#include "ops/BinopLiteral.h"


namespace pink {
    /**
     * @brief Represents the table of all known binops
     * 
     */
    class BinopTable {
    private:
        /**
         * @brief the table of registered binops
         * 
         */
        llvm::DenseMap<InternedString, std::unique_ptr<BinopLiteral>> table;

    public:
        /**
         * @brief Construct a new Binop Table
         * 
         */
        BinopTable();

        /**
         * @brief Destroy the Binop Table
         * 
         */
        ~BinopTable();

        /**
         * @brief Register a new binop in the table, with no implementations.
         * 
         * If the operator already exists, this function simply returns the existing binop
         * 
         * @param op the binary operator
         * @param p  the binops precedence
         * @param a  the binops associativity
         * @return std::pair<InternedString, BinopLiteral*> the binop that was added to the table
         */
        std::pair<InternedString, BinopLiteral*> Register(InternedString op, Precedence p, Associativity a);

        /**
         * @brief Register a new binop in the table, with one implementation
         * 
         * @param op the binary operator
         * @param p the binops precedence
         * @param a the binops associativity
         * @param left_t the left argument Type of the implementation
         * @param right_t the right argument Type of the implementation
         * @param ret_t the return Type of the implementation
         * @param fn the function which generates the implementation.
         * @return std::pair<InternedString, BinopLiteral*> the binop that was added to the table
         */
        std::pair<InternedString, BinopLiteral*> Register(InternedString op, Precedence p, Associativity a, Type* left_t, Type* right_t, Type* ret_t, BinopCodegenFn fn);

        /**
         * @brief Remove an existing binop from the table
         * 
         * if the op is not already registered, does nothing.
         * 
         * @param op the op to remove from the table
         */
        void Unregister(InternedString op);

        /**
         * @brief Lookup an existing binop in the table
         * 
         * @param op the binop to lookup
         * @return llvm::Optional<std::pair<InternedString, BinopLiteral*>> if has_value, then the binop from the table,
         * otherwise nothing.
         */
        llvm::Optional<std::pair<InternedString, BinopLiteral*>> Lookup(InternedString op);
    };
}
