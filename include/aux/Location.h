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
  class FirstLine {
  public:
    size_t data;

    FirstLine(int data) : data(data) {}

    auto operator==(const FirstLine &other) const -> bool {
      return data == other.data;
    }
  };

  class FirstColumn {
  public:
    size_t data;

    FirstColumn(int data) : data(data) {}

    auto operator==(const FirstColumn &other) const -> bool {
      return data == other.data;
    }
  };

  class LastLine {
  public:
    size_t data;

    LastLine(int data) : data(data) {}

    auto operator==(const LastLine &other) const -> bool {
      return data == other.data;
    }
  };

  class LastColumn {
  public:
    size_t data;

    LastColumn(int data) : data(data) {}

    auto operator==(const LastColumn &other) const -> bool {
      return data == other.data;
    }
  };

  /**
   * @brief holds the number of the first line that this Location appears on.
   *
   */
  FirstLine firstLine;

  /**
   * @brief holds the number of the first column that this Location appears on.
   *
   */
  FirstColumn firstColumn;

  /**
   * @brief holds the number of the last line that this Location appears on
   *
   */
  LastLine lastLine;

  /**
   * @brief holds the number of the last column that this Location appears on
   *
   */
  LastColumn lastColumn;

  /**
   * @brief Construct a new Location
   *
   * the default Location is (0, 0, 0, 0)
   */
  Location() : firstLine(0), firstColumn(0), lastLine(0), lastColumn(0) {}

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
  Location(FirstLine firstLine, FirstColumn firstColumn, LastLine lastLine,
           LastColumn lastColumn);

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
