/**
 * @file Location.h
 * @brief Header for class Location
 * @version 0.1
 *
 */
#pragma once
#include <cstddef> // size_t
#include <ostream>

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

auto operator<<(std::ostream &out, const Location &location) -> std::ostream &;

} // namespace pink
