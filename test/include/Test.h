#pragma once
#include <bitset>   // bitset
#include <cstddef>  // size_t
#include <iostream> // ostream

#include "support/Support.h" // bits_per_byte

/*
    This is a primitive Testing subroutine.
    it simply prints if the test Passed or Failed.
    then returns test.
*/
auto Test(std::ostream &out, const std::string &test_name, bool test) -> bool;

auto Test(std::ostream &out, const char *test_name, bool test) -> bool;


/* ---------------------- */
