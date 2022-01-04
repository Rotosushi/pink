#pragma once
#include <iostream> // ostream
#include <cstddef> // size_t

/*
    hex signifies 4 bits per char, thus we can
    always have at least 16 flags held in a size_t.
    on my machine, a size_t holds 64 bits, which means
    we can have 16 * 4 = 64 flags.

    T_1  = 0x0001
    T_2  = 0x0002
    T_3  = 0x0004
    T_4  = 0x0008
    T_5  = 0x0010
    T_6  = 0x0020
    T_7  = 0x0040
    T_8  = 0x0080
    T_9  = 0x0100
    T_10 = 0x0200
    T_11 = 0x0400
    T_12 = 0x0800
    T_13 = 0x1000
    T_14 = 0x2000
    T_15 = 0x4000
    T_16 = 0x8000
    ...

    64 bits would repeat the pattern for 64 bits, or
    0x0000'0000'0000'0000

*/

constexpr auto TEST_ERROR           = 0x0001;
constexpr auto TEST_STRING_INTERNER = 0x0002;

constexpr auto TEST_ALL             = 0xFFFF;


/*
    Test takes a flags variable which selects the particular
    test routines that are going to be evaluated. Then, for each
    test that passed or failed, the corresponding bit is set in
    the return value.
*/
size_t Test(std::ostream& out, size_t flags);
