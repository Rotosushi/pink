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
#include <iostream>
#include <string_view>

namespace pink {
[[noreturn]] inline void
FatalError(const char *dsc, const char *file, size_t line) {
  std::cerr << "Fatal Error in file: [" << file << "] line: [" << line << "]"
            << " description: [" << dsc << "]\n";
  exit(1);
}

[[noreturn]] inline void
FatalError(std::string_view dsc, const char *file, size_t line) {
  FatalError(dsc.begin(), file, line);
}

} // namespace pink