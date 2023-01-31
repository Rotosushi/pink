/**
 * @file PrecedenceAndAssociativity.h
 * @brief Header for Precedence and Associativity
 * @version 0.1
 *
 */
#pragma once
#include <cstdint> // int8_t

namespace pink {
enum class Associativity {
  None,
  Left,
  Right,
};

using Precedence = int8_t;
} // namespace pink
