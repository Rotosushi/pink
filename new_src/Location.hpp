#pragma once
#include <cstddef>

namespace pink {

class Location
{
public:
  std::size_t first_line;
  std::size_t first_column;
  std::size_t last_line;
  std::size_t last_column;

  Location();
  Location(const Location& loc);
  Location(std::size_t fl, std::size_t fc, std::size_t ll, std::size_t lc);
};

}
