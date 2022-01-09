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

    so this pattern is not infinitely extensible,
    so TODO: refactor the test framework to use
        vector<bool> for the flags, and InternedStrings
        for the Flag associations, instead of constexprs.

        this should allow arbitrarily many tests.
*/

// auxilliary tests
constexpr auto TEST_ERROR             = 0x0000'0001;
constexpr auto TEST_OUTCOME           = 0x0000'0002;
constexpr auto TEST_STRING_INTERNER   = 0x0000'0004;
constexpr auto TEST_SYMBOL_TABLE      = 0x0000'0008;
constexpr auto TEST_TYPE_INTERNER     = 0x0000'0010;
constexpr auto TEST_ENVIRONMENT       = 0x0000'0020;
constexpr auto TEST_PRECEDENCE_TABLE  = 0x0000'0040;
constexpr auto TEST_UNOP_CODEGEN      = 0x0000'0080;
constexpr auto TEST_UNOP_LITERAL      = 0x0000'0100;
// abstract syntax tree tests
constexpr auto TEST_AST_AND_NIL       = 0x0000'0200;
constexpr auto TEST_BOOL              = 0x0000'0400;
constexpr auto TEST_INT               = 0x0000'0800;
constexpr auto TEST_VARIABLE          = 0x0000'1000;
constexpr auto TEST_BIND              = 0x0000'2000;
constexpr auto TEST_BINOP             = 0x0000'4000;
constexpr auto TEST_UNOP              = 0x0000'8000;
// type tests
constexpr auto TEST_TYPE_AND_NIL_TYPE = 0x0001'0000;
constexpr auto TEST_INT_TYPE          = 0x0002'0000;
constexpr auto TEST_BOOL_TYPE         = 0x0004'0000;
// fronend tests
constexpr auto TEST_TOKEN             = 0x0008'0000;
constexpr auto TEST_LEXER             = 0x0010'0000;


constexpr auto TEST_ALL               = 0xFFFF'FFFF;


/*
    This is a primitive Testing subroutine.
    it simply prints if the test Passed or Failed.
    then returns test.
*/
bool Test(std::ostream& out, std::string test_name, bool test);
/*
    RunTests takes a flags variable which selects the particular
    test routines that are going to be evaluated. Then, for each
    test that passed or failed, the corresponding bit is set in
    the return value.
*/
size_t RunTests(std::ostream& out, size_t flags);
void PrintPassedTests(std::ostream& out, size_t flags);
