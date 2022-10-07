
#include "aux/Location.h"

namespace pink {

Location::Location(FirstLine firstLine, FirstColumn firstColumn,
                   LastLine lastLine, LastColumn lastColumn)
    : firstLine(firstLine), firstColumn(firstColumn), lastLine(lastLine),
      lastColumn(lastColumn) {}

auto Location::operator==(const Location &other) const -> bool {
  return (firstLine == other.firstLine) && (firstColumn == other.firstColumn) &&
         (lastLine == other.lastLine) && (lastColumn == other.lastColumn);
}
} // namespace pink
