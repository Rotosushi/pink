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
#include <source_location>
#include <string_view>
#include <system_error>

namespace pink {
/*
  "[function(line:column) file] FatalError [message]"
*/
[[noreturn]] inline void FatalError(
    std::string_view           message,
    std::source_location const location = std::source_location::current()) {
  std::cerr << "[" << location.function_name() << "(" << location.line() << ":"
            << location.column() << ") " << location.file_name() << "]"
            << " Fatal Error [" << message << "]\n";
  exit(EXIT_FAILURE);
}

[[noreturn]] inline void FatalError(
    std::error_code            errc,
    std::source_location const location = std::source_location::current()) {
  FatalError(errc.message(), location);
}
} // namespace pink