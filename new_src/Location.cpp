#include <cstddef>

#include "Location.hpp"

Location::Location()
{
  first_line   = 0;
  first_column = 0;
  last_line    = 0;
  last_column  = 0;
}

Location::Location(size_t fl, size_t fc, size_t ll, size_t lc)
{
  first_line   = fl;
  first_column = fc;
  last_line    = ll;
  last_column  = lc;
}
