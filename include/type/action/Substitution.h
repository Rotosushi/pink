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

#pragma once
#include "type/Type.h"

namespace pink {
/**
 * @brief Substitute source_type for target_name within target_type
 *
 * @param type_variable
 * @param source_type
 * @param target_type
 * @return Type::Pointer a Type representing the substituted Type.
 */
auto Substitution(Type::Pointer type_variable,
                  Type::Pointer source_type,
                  Type::Pointer target_type) noexcept -> Type::Pointer;
} // namespace pink
