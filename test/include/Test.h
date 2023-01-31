#pragma once
#include <bitset>   // bitset
#include <cstddef>  // size_t
#include <iostream> // ostream

#include "support/Support.h" // bits_per_byte

auto Test(std::ostream &out, const std::string &test_name, bool test) -> bool {
  if (test) {
    out << "\tTest: " << test_name << ": Passed\n";
  } else {
    out << "\tTest: " << test_name << ": Failed\n";
  }
  return test;
}

auto Test(std::ostream &out, std::string &&test_name, bool test) -> bool {
  if (test) {
    out << "\tTest: " << test_name << ": Passed\n";
  } else {
    out << "\tTest: " << test_name << ": Failed\n";
  }
  return test;
}

void TestHeader(std::ostream &out, std::string &test_name) {
  out << "------------------------------------------------";
  out << "Test: " << test_name << "\n";
}

auto TestFooter(std::ostream &out, std::string &test_name, bool test) -> bool {
  Test(out, test_name, test);
  out << "------------------------------------------------\n";
  return test;
}

/* ---------------------- */
