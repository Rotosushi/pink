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
#include <charconv>    // std::to_chars, std::from_chars
#include <string_view> // std::string_view

#include "aux/Outcome.h" // pink::Outcome

namespace pink {
template <typename T>
[[nodiscard]] inline auto ToNumber(std::string_view text)
    -> Outcome<T, std::errc> {
  T value;
  auto [ptr, errc]{std::from_chars(text.begin(), text.end(), value)};
  if (errc != std::errc{}) {
    return errc;
  }
  return value;
}
} // namespace pink
