/**
 * @file PrecedenceAndAssociativity.h
 * @brief Header for Precedence and Associativity
 * @version 0.1
 * 
 */
#pragma once
#include <cstdint> // int8_t


namespace pink {
    /**
     * @brief Represents the Associativity of binary operators
     * 
     */
    enum class Associativity {
        None,
        Left,
        Right,
    };

    /**
     * @brief Represents the Precedence of binary operators
     * 
     */
    typedef int8_t Precedence;
}
