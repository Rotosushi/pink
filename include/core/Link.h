/**
 * @file Link.h
 * @brief Header for the function Link
 * @version 0.1
 *
 */
#include "aux/Environment.h"

namespace pink {
/**
 * @brief Runs lld on the given Environment
 *
 *  This function calls lld::elf::link
 *
 * @param env the environment which emitted the object file to be linked
 */
auto Link(std::ostream &out, std::ostream &err, const Environment &env) -> int;
} // namespace pink
