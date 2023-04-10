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
 * @file StringInterner.h
 * @brief Header for class StringInterner
 * @version 0.1
 *
 */
#pragma once
#include "llvm/ADT/StringSet.h"

namespace pink {
using InternedString = char const *;

class StringInterner {
private:
  llvm::StringSet<> set;

public:
  inline auto Intern(std::string_view str) -> InternedString {
    return set.try_emplace(str).first->getKey().data();
  }
};

} // namespace pink
