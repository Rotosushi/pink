/**
 * @file Link.h
 * @brief Header for the function Link
 * @version 0.1
 *
 */
#include "aux/Environment.h"

namespace pink {
/**
 * @brief Runs the Linker lld on the given compilation unit
 *
 *  This function calls lld::elf::link
 *
 * @param env the environment which emitted the object file to be linked
 */
void Link(const Environment &env);
} // namespace pink
