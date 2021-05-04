#pragma once
#include <cstddef>

class Location
{
public:
  size_t first_line;
  size_t first_column;
  size_t last_line;
  size_t last_column;

  Location();
  Location(size_t fl, size_t fc, size_t ll, size_t lc);
};
