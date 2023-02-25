/**
 * @file Compile.h
 * @brief Header for the function Compile
 * @version 0.1
 *
 */
#pragma once

#include "aux/Environment.h"
/**
 * @brief The namespace for the entire project
 *
 * \todo where is the best place to put the documentation for the whole
 * namespace?
 *
 */
namespace pink {
/**
 * @brief Compile the source file represented by the given
 * Environment.
 *
 * @param argc
 * @param argv
 */
auto Compile(int argc, char **argv) -> int;
} // namespace pink
