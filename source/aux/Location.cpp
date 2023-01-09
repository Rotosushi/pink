
#include "aux/Location.h"

namespace pink {
auto operator<<(std::ostream &out, const Location &location) -> std::ostream & {
  out << "{" << location.firstLine << ", " << location.firstColumn << ", "
      << location.lastLine << ", " << location.lastColumn << "}";
  return out;
}

} // namespace pink
