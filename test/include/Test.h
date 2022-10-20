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

class Testbench {
public:
  enum Which : size_t {
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
    flags,
    location,

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
    array,
    tuple,
    conditional,
    dot,
    loop,

    type,
    /* type tests */
    nil_type,
    boolean_type,
    integer_type,
    function_type,
    array_type,
    pointer_type,
    void_type,
    tuple_type,

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
  std::bitset<sizeof(Which) * pink::bits_per_byte> tests_run;
  std::bitset<sizeof(Which) * pink::bits_per_byte> test_results;

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
