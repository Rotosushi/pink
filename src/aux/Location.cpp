
#include "aux/Location.h"

namespace pink {

Location::Location(size_t firstLine, size_t firstColumn, size_t lastLine,
                   size_t lastColumn)
    : firstLine(firstLine), firstColumn(firstColumn), lastLine(lastLine),
      lastColumn(lastColumn) {}

auto Location::operator==(const Location &other) const -> bool {
  bool result = false;
  result = (firstLine == other.firstLine) &&
           (firstColumn == other.firstColumn) && (lastLine == other.lastLine) &&
           (lastColumn == other.lastColumn);
  return result;
}
} // namespace pink
