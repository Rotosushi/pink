/**
 * @file BinopPrimitives.h
 * @brief Header for Binop Primitives
 * @version 0.1
 *
 */
#pragma once

namespace pink {
class Environment;
/**
 * @brief Initialize the [BinopTable](#BinopTable) with all predefined binops
 *
 * @param env The env holding the BinopTable to initialize
 */
void InitializeBinopPrimitives(Environment &env);
} // namespace pink
