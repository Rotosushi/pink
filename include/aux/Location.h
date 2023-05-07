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
 * @file Location.h
 * @brief Header for class Location
 * @version 0.1
 *
 */
#pragma once
#include <cstddef> // std::size_t
#include <ostream> // std::ostream

namespace pink {
/**
 * @brief represents a single textual location
 *
 *
 * if we have a line of text within an array starting at 0:
 * (and we just started parsing the n'th line of text)
 *
 *  0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
 *  --|---|---|---|---|---|---|---|
 *  s | y | m | b | o | l | ~ | ~ |
 *
 * then the textual location of the term 'symbol' would be
 * firstLine   == n
 * firstColumn == 0
 * lastLine    == n
 * lastColumn  == 6
 *
 */
class Location {
public:
  std::size_t firstLine;
  std::size_t firstColumn;
  std::size_t lastLine;
  std::size_t lastColumn;

  constexpr Location()
      : firstLine(0),
        firstColumn(0),
        lastLine(0),
        lastColumn(0) {}
  ~Location() = default;
  constexpr inline Location(std::size_t firstLine,
                            std::size_t firstColumn,
                            std::size_t lastLine,
                            std::size_t lastColumn) noexcept
      : firstLine(firstLine),
        firstColumn(firstColumn),
        lastLine(lastLine),
        lastColumn(lastColumn) {}
  constexpr Location(const Location &other) noexcept                = default;
  constexpr inline Location(Location &&other) noexcept              = default;
  constexpr auto operator=(Location &&other) noexcept -> Location & = default;
  constexpr auto operator=(const Location &other) -> Location     & = default;

  constexpr inline auto operator==(const Location &other) const -> bool {
    return (firstLine == other.firstLine) &&
           (firstColumn == other.firstColumn) && (lastLine == other.lastLine) &&
           (lastColumn == other.lastColumn);
  }

  friend auto operator<<(std::ostream &out, const Location &location)
      -> std::ostream &;
};

inline auto operator<<(std::ostream &out, const Location &location)
    -> std::ostream & {
  out << "{" << location.firstLine << ", " << location.firstColumn << ", "
      << location.lastLine << ", " << location.lastColumn << "}";
  return out;
}

} // namespace pink
