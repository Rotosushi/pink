
#include "Test.h"


auto Test(std::ostream &out, const std::string &test_name, bool test) -> bool {
  if (test) {
    out << "\tTest: " << test_name << ": Passed\n";
  } else {
    out << "\tTest: " << test_name << ": Failed\n";
  }

  return test;
}

auto Test(std::ostream &out, const char *test_name, bool test) -> bool {
  if (test) {
    out << "\tTest: " << test_name << ": Passed\n";
  } else {
    out << "\tTest: " << test_name << ": Failed\n";
  }

  return test;
}
