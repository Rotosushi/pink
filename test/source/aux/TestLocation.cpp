#include "Test.h"

#include "aux/TestLocation.h"

#include "aux/Location.h"

auto TestLocation(std::ostream &out) -> bool {
  bool result = true;
  out << "\n-----------------------\n";
  out << "Testing pink::Location\n";

  pink::Location location = {1, 2, 3, 4};

  result &= Test(out, "Location::firstLine", location.firstLine == 1);
  result &= Test(out, "Location::firstColumn", location.firstColumn == 2);
  result &= Test(out, "Location::lastLine", location.lastLine == 3);
  result &= Test(out, "Location::lastColumn", location.lastColumn == 4);

  return Test(out, "pink::Location", result);
}