// Copyright (C) 2023 cadence
//
// This file is part of pink.
//
// pink is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// pink is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with pink.  If not, see <http://www.gnu.org/licenses/>.

/**
 * @file Link.h
 * @brief Header for the function Link
 * @version 0.1
 *
 */
#include "aux/Environment.h"

namespace pink {
/**
 * @brief Runs lld on the given CompilationUnit
 *
 *  This function calls lld::elf::link
 *
 * @param env the environment which emitted the object file to be linked
 */
auto Link(std::ostream &out, std::ostream &err, const CompilationUnit &env)
    -> int;
} // namespace pink
