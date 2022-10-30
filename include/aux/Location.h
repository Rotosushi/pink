/**
 * @file Location.h
 * @brief Header for class Location
 * @version 0.1
 *
 */
#pragma once
#include <cstddef> // size_t

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
  /**
   * @brief holds the number of the first line that this Location appears on.
   *
   */
  size_t firstLine;

  /**
   * @brief holds the number of the first column that this Location appears on.
   *
   */
  size_t firstColumn;

  /**
   * @brief holds the number of the last line that this Location appears on
   *
   */
  size_t lastLine;

  /**
   * @brief holds the number of the last column that this Location appears on
   *
   */
  size_t lastColumn;

  /**
   * @brief Construct a new Location
   *
   * the default Location is (0, 0, 0, 0)
   */
  constexpr Location()
      : firstLine(0), firstColumn(0), lastLine(0), lastColumn(0) {}

  /**
   * @brief
   *
   *
   * @param other the other Location to copy
   */
  constexpr Location(const Location &other) = default;

  /**
   * @brief Construct a new Location
   *
   * @param firstLine the firstLine
   * @param firstColumn the firstColumn
   * @param lastLine the lastLine
   * @param lastColumn the lastColumn
   */
  // NOLINTBEGIN(bugprone-easily-swappable-parameters)
  // There is no better representation of a location than
  // four integral types. We have to specify all of them.
  // Unless we want four trival classes within this
  // already trivial class, which all provide overloads
  // to be easily constructed from integer literals, which
  // by having defeats the purpose of the lint in the first place.
  // we are simply going to ignore this lint here.
  constexpr inline Location(size_t firstLine, size_t firstColumn,
                            size_t lastLine, size_t lastColumn)
      : firstLine(firstLine), firstColumn(firstColumn), lastLine(lastLine),
        lastColumn(lastColumn) {}
  // NOLINTEND(bugprone-easily-swappable-parameters)

  constexpr inline Location(Location &&other) noexcept = default;

  ~Location() = default;

  constexpr auto operator=(Location &&other) noexcept -> Location & = default;
  /**
   * @brief Assigns this Location to the value of another Location
   *
   * @param other
   * @return Location&
   */
  constexpr auto operator=(const Location &other) -> Location & = default;

  /**
   * @brief compares two locations for equality
   *
   * two locations are equal if and only if each member of each location is
   * equal.
   *
   * @param other the Location to compare against this Location
   * @return true iff this Location is equal to the other Location
   * @return false iff this Location is not equal to the other Location
   */
  constexpr inline auto operator==(const Location &other) const -> bool {
    return (firstLine == other.firstLine) &&
           (firstColumn == other.firstColumn) && (lastLine == other.lastLine) &&
           (lastColumn == other.lastColumn);
  }
};
} // namespace pink
