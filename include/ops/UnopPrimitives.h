/**
 * @file UnopPrimitives.h
 * @brief Header for InitializeUnopPrimitives
 * @version 0.1
 */
#pragma once

namespace pink {
class Environment;
/**
 * @brief Initialize the [UnopTable](#UnopTable) with all predefined unops
 *
 * @param env the env holding the UnopTable to initialize
 */
void InitializeUnopPrimitives(Environment &env);
} // namespace pink
