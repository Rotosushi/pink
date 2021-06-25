#include <cstddef>

#include "Location.hpp"

namespace pink {

Location::Location()
{
  first_line   = 0;
  first_column = 0;
  last_line    = 0;
  last_column  = 0;
}

Location::Location(const Location& loc)
  : first_line(loc.first_line),
    first_column(loc.first_column),
    last_line(loc.last_line),
    last_column(loc.last_column) 
{

}

Location::Location(std::size_t fl, std::size_t fc, std::size_t ll, std::size_t lc)
{
  first_line   = fl;
  first_column = fc;
  last_line    = ll;
  last_column  = lc;
}

}
