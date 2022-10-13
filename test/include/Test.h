#pragma once
#include <bitset>   // bitset
#include <cstddef>  // size_t
#include <iostream> // ostream

#include "support/Support.h" // bits_per_byte

/*
    hex signifies 4 bits per char, thus (per the standard) we can
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

    this pattern is not infinitely extensible,
    so TODO: refactor the test framework to use
        vector<bool> for the flags, and InternedStrings
        for the Flag associations, instead of constexprs.

        this should allow arbitrarily many tests.
*/

// auxilliary tests
constexpr auto TEST_ERROR = 0x0000'0000'0001;
constexpr auto TEST_OUTCOME = 0x0000'0000'0002;
constexpr auto TEST_STRING_INTERNER = 0x0000'0000'0004;
constexpr auto TEST_SYMBOL_TABLE = 0x0000'0000'0008;
constexpr auto TEST_TYPE_INTERNER = 0x0000'0000'0010;
constexpr auto TEST_ENVIRONMENT = 0x0000'0000'0020;
constexpr auto TEST_UNOP_CODEGEN = 0x0000'0000'0040;
constexpr auto TEST_UNOP_LITERAL = 0x0000'0000'0080;
constexpr auto TEST_UNOP_TABLE = 0x0000'0000'0100;
constexpr auto TEST_BINOP_CODEGEN = 0x0000'0000'0200;
constexpr auto TEST_BINOP_LITERAL = 0x0000'0000'0400;
constexpr auto TEST_BINOP_TABLE = 0x0000'0000'0800;
// abstract syntax tree tests
constexpr auto TEST_AST_AND_NIL = 0x0000'0000'1000;
constexpr auto TEST_BOOL = 0x0000'0000'2000;
constexpr auto TEST_INT = 0x0000'0000'4000;
constexpr auto TEST_VARIABLE = 0x0000'0000'8000;
constexpr auto TEST_BIND = 0x0000'0001'0000;
constexpr auto TEST_BINOP = 0x0000'0002'0000;
constexpr auto TEST_UNOP = 0x0000'0004'0000;

constexpr auto TEST_ASSIGNMENT = 0x0000'0008'0000;
constexpr auto TEST_BLOCK = 0x0000'0010'0000;
constexpr auto TEST_FUNCTION = 0x0000'0020'0000;
constexpr auto TEST_APPLICATION = 0x0000'0040'0000;
// type tests
constexpr auto TEST_TYPE_AND_NIL_TYPE = 0x0000'0080'0000;
constexpr auto TEST_INT_TYPE = 0x0000'0100'0000;
constexpr auto TEST_BOOL_TYPE = 0x0000'0200'0000;
constexpr auto TEST_FUNCTION_TYPE = 0x0000'0400'0000;
// frontend tests
constexpr auto TEST_TOKEN = 0x0000'0800'0000;
constexpr auto TEST_LEXER = 0x0000'1000'0000;
constexpr auto TEST_PARSER = 0x0000'2000'0000;
// kernel tests
constexpr auto TEST_UNOP_PRIMITIVES = 0x0000'4000'0000;
constexpr auto TEST_BINOP_PRIMITIVES = 0x0000'8000'0000;
// core tests
constexpr auto TEST_TYPECHECK = 0x0001'0000'0000;
constexpr auto TEST_CODEGEN = 0x0002'0000'0000;

constexpr auto TEST_ALL = 0xFFFF'FFFF'FFFF;

/*
    This is a primitive Testing subroutine.
    it simply prints if the test Passed or Failed.
    then returns test.
*/
auto Test(std::ostream &out, const std::string &test_name, bool test) -> bool;

auto Test(std::ostream &out, const char *test_name, bool test) -> bool;
/*
    RunTests takes a flags variable which selects the particular
    test routines that are going to be evaluated. Then, for each
    test that passed or failed, the corresponding bit is set in
    the return value.

    #TODO: refactor RunTests, and each unit test function to take
    an Environment&, so we only have to run that initialization code
    once.
*/
auto RunTests(std::ostream &out, size_t which_tests) -> size_t;

class Testbench {
public:
  enum Which {
    all,

    aux,
    /* auxilliary tests */
    error,
    outcome,
    string_interner,
    symbol_table,
    type_interner,
    environment,
    unop_codegen,
    unop_literal,
    unop_table,
    binop_codegen,
    binop_literal,
    binop_table,

    ast,
    /* ast tests */
    nil,
    boolean,
    integer,
    variable,
    bind,
    binop,
    unop,
    assignment,
    block,
    function,
    application,

    type,
    /* type tests */
    nil_type,
    boolean_type,
    integer_type,
    function_type,

    kernel,
    /* kernel tests */
    unop_primitives,
    binop_primitives,

    front,
    /* frontend tests */
    token,
    lexer,
    parser,

    core,
    /* core tests */
    typecheck,
    codegen,
  };

private:
  std::bitset<sizeof(Which) * bits_per_byte> tests_run;
  std::bitset<sizeof(Which) * bits_per_byte> test_results;

public:
  auto SetTestsRun(Which which, bool state) -> bool;
  auto SetTestRun(Which which, bool state) -> bool;
  auto SetTestsResult(Which which, bool state) -> bool;
  auto SetTestResult(Which which, bool state) -> bool;
  [[nodiscard]] auto RanTest(Which which) const -> bool;
  [[nodiscard]] auto GetTestResult(Which which) const -> bool;

  void PrintPassedTests(std::ostream &out) const;
  void PrintAuxPassedTests(std::ostream &out) const;
  void PrintAstPassedTests(std::ostream &out) const;
  void PrintTypePassedTests(std::ostream &out) const;
  void PrintKernelPassedTests(std::ostream &out) const;
  void PrintFrontendPassedTests(std::ostream &out) const;
  void PrintCorePassedTests(std::ostream &out) const;

  [[nodiscard]] auto RunTests(std::ostream &out) -> bool;
  [[nodiscard]] auto RunAuxTests(std::ostream &out) -> bool;
  [[nodiscard]] auto RunAstTests(std::ostream &out) -> bool;
  [[nodiscard]] auto RunTypeTests(std::ostream &out) -> bool;
  [[nodiscard]] auto RunKernelTests(std::ostream &out) -> bool;
  [[nodiscard]] auto RunFrontTests(std::ostream &out) -> bool;
  [[nodiscard]] auto RunCoreTests(std::ostream &out) -> bool;
};

/* ---------------------- */
