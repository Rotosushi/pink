/**
 * @file UnopPrimitives.h
 * @brief Header for InitializeUnopPrimitives
 * @version 0.1
 */
#pragma once
#include "aux/Environment.h"

namespace pink {
/**
 * @brief Initialize the [UnopTable](#UnopTable) with all predefined unops
 *
 * @param env the env holding the UnopTable to initialize
 */
void InitializeUnopPrimitives(Environment &env);
} // namespace pink
