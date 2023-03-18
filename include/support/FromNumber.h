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
#include <array>    // std::array<T, N>
#include <charconv> // std::to_chars, std::from_chars
#include <concepts> // std::integral
#include <limits>   // std::numeric_limits<T>::digits10
#include <string>   // std::string

#include "aux/Outcome.h" // pink::Outcome

namespace pink {
template <std::integral T>
[[nodiscard]] inline auto FromNumber(T number)
    -> Outcome<std::string, std::errc> {
  std::array<char, std::numeric_limits<T>::digits10 + 1> buffer{};
  auto [ptr, errc]{std::to_chars(buffer.begin(), buffer.end(), number)};
  if (errc != std::errc{}) {
    return errc;
  }
  return std::string{buffer.begin()};
}

template <std::floating_point T>
[[nodiscard]] inline auto FromNumber(T number)
    -> Outcome<std::string, std::errc> {
  std::array<char, std::numeric_limits<T>::max_digits10 + 1> buffer{};
  auto [ptr, errc] = std::to_chars(buffer.begin(), buffer.end(), number);
  if (errc != std::errc{}) {
    return errc;
  }
  return std::string{buffer.begin()};
}
} // namespace pink
