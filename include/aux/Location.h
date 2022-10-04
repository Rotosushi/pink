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
  Location() = default;

  /**
   * @brief Construct a new Location from another Location
   *
   * @param other the other Location to copy
   */
  Location(const Location &other) = default;

  /**
   * @brief Construct a new Location
   *
   * @param firstLine the firstLine
   * @param firstColumn the firstColumn
   * @param lastLine the lastLine
   * @param lastColumn the lastColumn
   */
  Location(size_t firstLine, size_t firstColumn, size_t lastLine,
           size_t lastColumn);

  /**
   * @brief Assigns this Location to the value of another Location
   *
   * @param other
   * @return Location&
   */
  auto operator=(const Location &other) -> Location & = default;

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
  auto operator==(const Location &other) const -> bool;
};

} // namespace pink
