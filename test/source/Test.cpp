
#include "Test.h"

#include "aux/TestEnvironment.h"
#include "aux/TestError.h"
#include "aux/TestFlags.h"
#include "aux/TestLocation.h"
#include "aux/TestOutcome.h"
#include "aux/TestStringInterner.h"
#include "aux/TestSymbolTable.h"
#include "aux/TestTypeInterner.h"

#include "ops/TestBinopCodegen.h"
#include "ops/TestBinopLiteral.h"
#include "ops/TestBinopTable.h"
#include "ops/TestUnopCodegen.h"
#include "ops/TestUnopLiteral.h"
#include "ops/TestUnopTable.h"

#include "ast/TestApplication.h"
#include "ast/TestArray.h"
#include "ast/TestAssignment.h"
#include "ast/TestAstAndNil.h"
#include "ast/TestBind.h"
#include "ast/TestBinop.h"
#include "ast/TestBlock.h"
#include "ast/TestBool.h"
#include "ast/TestConditional.h"
#include "ast/TestDot.h"
#include "ast/TestFunction.h"
#include "ast/TestInt.h"
#include "ast/TestTuple.h"
#include "ast/TestUnop.h"
#include "ast/TestVariable.h"
#include "ast/TestWhile.h"

#include "type/TestArrayType.h"
#include "type/TestBoolType.h"
#include "type/TestFunctionType.h"
#include "type/TestIntType.h"
#include "type/TestPointerType.h"
#include "type/TestTupleType.h"
#include "type/TestTypeAndNilType.h"
#include "type/TestVoidType.h"

#include "front/TestLexer.h"
#include "front/TestParser.h"
#include "front/TestToken.h"

#include "kernel/TestBinopPrimitives.h"
#include "kernel/TestUnopPrimitives.h"

#include "core/TestBasics.h"
#include "core/TestFirstPhase.h"

/*
    This is a super basic funclet, which simply saves
    on typing out so much code in every
    Test...() body
*/
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

auto Testbench::SetTestsRun(Testbench::Which which, bool state) -> bool {
  if (which == Testbench::all) {
    if (state) {
      tests_run.set();
    } else {
      tests_run.reset();
    }
    return state;
  }

  if (which == Testbench::aux) {
    tests_run[Testbench::error] = state;
    tests_run[Testbench::outcome] = state;
    tests_run[Testbench::string_interner] = state;
    tests_run[Testbench::symbol_table] = state;
    tests_run[Testbench::type_interner] = state;
    tests_run[Testbench::environment] = state;
    tests_run[Testbench::unop_codegen] = state;
    tests_run[Testbench::unop_literal] = state;
    tests_run[Testbench::unop_table] = state;
    tests_run[Testbench::binop_codegen] = state;
    tests_run[Testbench::binop_literal] = state;
    tests_run[Testbench::binop_table] = state;
    tests_run[Testbench::flags] = state;
    tests_run[Testbench::location] = state;
    return state;
  }

  if (which == Testbench::ast) {
    tests_run[Testbench::nil] = state;
    tests_run[Testbench::boolean] = state;
    tests_run[Testbench::integer] = state;
    tests_run[Testbench::variable] = state;
    tests_run[Testbench::bind] = state;
    tests_run[Testbench::binop] = state;
    tests_run[Testbench::unop] = state;
    tests_run[Testbench::assignment] = state;
    tests_run[Testbench::block] = state;
    tests_run[Testbench::function] = state;
    tests_run[Testbench::application] = state;
    tests_run[Testbench::array] = state;
    tests_run[Testbench::tuple] = state;
    tests_run[Testbench::conditional] = state;
    tests_run[Testbench::dot] = state;
    tests_run[Testbench::loop] = state;
    return state;
  }

  if (which == Testbench::type) {
    tests_run[Testbench::nil_type] = state;
    tests_run[Testbench::boolean_type] = state;
    tests_run[Testbench::integer_type] = state;
    tests_run[Testbench::function_type] = state;
    tests_run[Testbench::array_type] = state;
    tests_run[Testbench::pointer_type] = state;
    tests_run[Testbench::void_type] = state;
    tests_run[Testbench::tuple_type] = state;
    return state;
  }

  if (which == Testbench::kernel) {
    tests_run[Testbench::unop_primitives] = state;
    tests_run[Testbench::binop_primitives] = state;
    return state;
  }

  if (which == Testbench::front) {
    tests_run[Testbench::token] = state;
    tests_run[Testbench::lexer] = state;
    tests_run[Testbench::parser] = state;
    return state;
  }

  if (which == Testbench::core) {
    tests_run[Testbench::typecheck] = state;
    tests_run[Testbench::codegen] = state;
    return state;
  }

  return tests_run[which] = state;
}

auto Testbench::SetTestRun(Testbench::Which which, bool state) -> bool {
  return tests_run[which] = state;
}

auto Testbench::SetTestsResult(Testbench::Which which, bool state) -> bool {
  if (which == Testbench::all) {
    if (state) {
      test_results.set();
    } else {
      test_results.reset();
    }
    return state;
  }

  if (which == Testbench::aux) {
    test_results[Testbench::error] = state;
    test_results[Testbench::outcome] = state;
    test_results[Testbench::string_interner] = state;
    test_results[Testbench::symbol_table] = state;
    test_results[Testbench::type_interner] = state;
    test_results[Testbench::environment] = state;
    test_results[Testbench::unop_codegen] = state;
    test_results[Testbench::unop_literal] = state;
    test_results[Testbench::unop_table] = state;
    test_results[Testbench::binop_codegen] = state;
    test_results[Testbench::binop_literal] = state;
    test_results[Testbench::binop_table] = state;
    test_results[Testbench::flags] = state;
    test_results[Testbench::location] = state;
    return state;
  }

  if (which == Testbench::ast) {
    test_results[Testbench::nil] = state;
    test_results[Testbench::boolean] = state;
    test_results[Testbench::integer] = state;
    test_results[Testbench::variable] = state;
    test_results[Testbench::bind] = state;
    test_results[Testbench::binop] = state;
    test_results[Testbench::unop] = state;
    test_results[Testbench::assignment] = state;
    test_results[Testbench::block] = state;
    test_results[Testbench::function] = state;
    test_results[Testbench::application] = state;
    test_results[Testbench::array] = state;
    test_results[Testbench::tuple] = state;
    test_results[Testbench::conditional] = state;
    test_results[Testbench::dot] = state;
    test_results[Testbench::loop] = state;
    return state;
  }

  if (which == Testbench::type) {
    test_results[Testbench::nil_type] = state;
    test_results[Testbench::boolean_type] = state;
    test_results[Testbench::integer_type] = state;
    test_results[Testbench::function_type] = state;
    test_results[Testbench::array_type] = state;
    test_results[Testbench::pointer_type] = state;
    test_results[Testbench::void_type] = state;
    test_results[Testbench::tuple_type] = state;
    return state;
  }

  if (which == Testbench::kernel) {
    test_results[Testbench::unop_primitives] = state;
    test_results[Testbench::binop_primitives] = state;
    return state;
  }

  if (which == Testbench::front) {
    test_results[Testbench::token] = state;
    test_results[Testbench::lexer] = state;
    test_results[Testbench::parser] = state;
    return state;
  }

  if (which == Testbench::core) {
    test_results[Testbench::typecheck] = state;
    test_results[Testbench::codegen] = state;
    return state;
  }

  return test_results[which] = state;
}

auto Testbench::SetTestResult(Testbench::Which which, bool state) -> bool {
  return test_results[which] = state;
}

auto Testbench::RanTest(Testbench::Which which) const -> bool {
  return tests_run[which];
}

auto Testbench::GetTestResult(Testbench::Which which) const -> bool {
  return test_results[which];
}

void Testbench::PrintPassedTests(std::ostream &out) const {
  PrintAuxPassedTests(out);
  PrintAstPassedTests(out);
  PrintTypePassedTests(out);
  PrintKernelPassedTests(out);
  PrintFrontendPassedTests(out);
  PrintCorePassedTests(out);

  out << "\nsynopsis:\n";
  if (RanTest(Testbench::aux)) {
    Test(out, "auxilliary tests", GetTestResult(Testbench::aux));
  }

  if (RanTest(Testbench::ast)) {
    Test(out, "abstract syntax tree tests", GetTestResult(Testbench::ast));
  }

  if (RanTest(Testbench::type)) {
    Test(out, "type tests", GetTestResult(Testbench::type));
  }

  if (RanTest(Testbench::kernel)) {
    Test(out, "kernel tests", GetTestResult(Testbench::kernel));
  }

  if (RanTest(Testbench::front)) {
    Test(out, "front tests", GetTestResult(Testbench::front));
  }

  if (RanTest(Testbench::core)) {
    Test(out, "core tests", GetTestResult(Testbench::core));
  }
}

void Testbench::PrintAuxPassedTests(std::ostream &out) const {
  bool result = true;
  if (RanTest(Testbench::aux)) {
    out << "auxilliary tests: \n";
  }

  if (RanTest(Testbench::error)) {
    result &= Test(out, "pink::Error", GetTestResult(Testbench::error));
  }
  if (RanTest(Testbench::outcome)) {
    result &= Test(out, "pink::Outcome", GetTestResult(Testbench::outcome));
  }
  if (RanTest(Testbench::string_interner)) {
    result &= Test(out, "pink::StringInterner",
                   GetTestResult(Testbench::string_interner));
  }
  if (RanTest(Testbench::symbol_table)) {
    result &=
        Test(out, "pink::SymbolTable", GetTestResult(Testbench::symbol_table));
  }
  if (RanTest(Testbench::type_interner)) {
    result &= Test(out, "pink::TypeInterner",
                   GetTestResult(Testbench::type_interner));
  }
  if (RanTest(Testbench::environment)) {
    result &=
        Test(out, "pink::Environment", GetTestResult(Testbench::environment));
  }
  if (RanTest(Testbench::unop_codegen)) {
    result &=
        Test(out, "pink::UnopCodegen", GetTestResult(Testbench::unop_codegen));
  }
  if (RanTest(Testbench::unop_literal)) {
    result &=
        Test(out, "pink::UnopLiteral", GetTestResult(Testbench::unop_literal));
  }
  if (RanTest(Testbench::unop_table)) {
    result &=
        Test(out, "pink::UnopTable", GetTestResult(Testbench::unop_table));
  }
  if (RanTest(Testbench::binop_codegen)) {
    result &= Test(out, "pink::BinopCodegen",
                   GetTestResult(Testbench::binop_codegen));
  }
  if (RanTest(Testbench::binop_literal)) {
    result &=
        Test(out, "pink::BinopLiteral", RanTest(Testbench::binop_literal));
  }
  if (RanTest(Testbench::binop_table)) {
    result &=
        Test(out, "pink::BinopTable", GetTestResult(Testbench::binop_table));
  }
  if (RanTest(Testbench::flags)) {
    result &=
        Test(out, "pink::TypecheckFlags", GetTestResult(Testbench::flags));
  }
  if (RanTest(Testbench::location)) {
    result &= Test(out, "pink::Location", GetTestResult(Testbench::location));
  }

  Test(out, "auxilliary tests", result);
}

void Testbench::PrintAstPassedTests(std::ostream &out) const {
  bool result = true;
  if (RanTest(Testbench::ast)) {
    out << "abstract syntax tree tests: \n";
  }

  if (RanTest(Testbench::nil)) {
    result &= Test(out, "pink::Ast, pink::Nil", GetTestResult(Testbench::nil));
  }
  if (RanTest(Testbench::boolean)) {
    result &= Test(out, "pink::Bool", GetTestResult(Testbench::boolean));
  }
  if (RanTest(Testbench::integer)) {
    result &= Test(out, "pink::Int", GetTestResult(Testbench::integer));
  }
  if (RanTest(Testbench::variable)) {
    result &= Test(out, "pink::Variable", GetTestResult(Testbench::variable));
  }
  if (RanTest(Testbench::bind)) {
    result &= Test(out, "pink::Bind", GetTestResult(Testbench::bind));
  }
  if (RanTest(Testbench::binop)) {
    result &= Test(out, "pink::Binop", GetTestResult(Testbench::binop));
  }
  if (RanTest(Testbench::unop)) {
    result &= Test(out, "pink::Unop", GetTestResult(Testbench::unop));
  }
  if (RanTest(Testbench::assignment)) {
    result &=
        Test(out, "pink::Assignment", GetTestResult(Testbench::assignment));
  }
  if (RanTest(Testbench::block)) {
    result &= Test(out, "pink::Block", GetTestResult(Testbench::block));
  }
  if (RanTest(Testbench::function)) {
    result &= Test(out, "pink::Function", GetTestResult(Testbench::function));
  }
  if (RanTest(Testbench::application)) {
    result &=
        Test(out, "pink::Application", GetTestResult(Testbench::application));
  }
  if (RanTest(Testbench::array)) {
    result &= Test(out, "pink::Array", GetTestResult(Testbench::array));
  }
  if (RanTest(Testbench::tuple)) {
    result &= Test(out, "pink::Tuple", GetTestResult(Testbench::tuple));
  }
  if (RanTest(Testbench::conditional)) {
    result &=
        Test(out, "pink::Conditional", GetTestResult(Testbench::conditional));
  }
  if (RanTest(Testbench::dot)) {
    result &= Test(out, "pink::Dot", GetTestResult(Testbench::dot));
  }
  if (RanTest(Testbench::loop)) {
    result &= Test(out, "pink::While", GetTestResult(Testbench::loop));
  }

  Test(out, "abstract syntax tree tests", result);
}

void Testbench::PrintTypePassedTests(std::ostream &out) const {
  bool result = true;
  if (RanTest(Testbench::type)) {
    out << "type tests: \n";
  }

  if (RanTest(Testbench::nil_type)) {
    result &= Test(out, "pink::Type, pink::NilType",
                   GetTestResult(Testbench::nil_type));
  }
  if (RanTest(Testbench::integer_type)) {
    result &=
        Test(out, "pink::IntType", GetTestResult(Testbench::integer_type));
  }
  if (RanTest(Testbench::boolean_type)) {
    result &=
        Test(out, "pink::BoolType", GetTestResult(Testbench::boolean_type));
  }
  if (RanTest(Testbench::function_type)) {
    result &= Test(out, "pink::FunctionType",
                   GetTestResult(Testbench::function_type));
  }
  if (RanTest(Testbench::array_type)) {
    result &=
        Test(out, "pink::ArrayType", GetTestResult(Testbench::array_type));
  }

  if (RanTest(Testbench::pointer_type)) {
    result &=
        Test(out, "pink::PointerType", GetTestResult(Testbench::pointer_type));
  }

  if (RanTest(Testbench::void_type)) {
    result &= Test(out, "pink::VoidType", GetTestResult(Testbench::void_type));
  }

  if (RanTest(Testbench::tuple_type)) {
    result &=
        Test(out, "pink::TupleType", GetTestResult(Testbench::tuple_type));
  }

  Test(out, "type tests", result);
}

void Testbench::PrintKernelPassedTests(std::ostream &out) const {
  bool result = true;

  if (RanTest(Testbench::kernel)) {
    out << "kernel tests: \n";
  }

  if (RanTest(Testbench::unop_primitives)) {
    result &= Test(out, "pink::UnopPrimitives",
                   GetTestResult(Testbench::unop_primitives));
  }
  if (RanTest(Testbench::binop_primitives)) {
    result &= Test(out, "pink::BinopPrimitives",
                   GetTestResult(Testbench::binop_primitives));
  }
  Test(out, "kernel tests", result);
}

void Testbench::PrintFrontendPassedTests(std::ostream &out) const {
  bool result = true;
  if (RanTest(Testbench::front)) {
    out << "frontend tests: \n";
  }

  if (RanTest(Testbench::token)) {
    result &= Test(out, "pink::Token", GetTestResult(Testbench::token));
  }
  if (RanTest(Testbench::lexer)) {
    result &= Test(out, "pink::Lexer", GetTestResult(Testbench::lexer));
  }
  if (RanTest(Testbench::parser)) {
    result &= Test(out, "pink::Parser", GetTestResult(Testbench::parser));
  }

  Test(out, "frontend tests", result);
}

void Testbench::PrintCorePassedTests(std::ostream &out) const {
  bool result = true;
  if (RanTest(Testbench::core)) {
    out << "core tests: \n";
  }

  if (RanTest(Testbench::typecheck)) {
    result &=
        Test(out, "pink::Ast::Typecheck", GetTestResult(Testbench::typecheck));
  }
  if (RanTest(Testbench::codegen)) {
    result &=
        Test(out, "pink::Ast::Codegen", GetTestResult(Testbench::codegen));
  }

  Test(out, "core tests", result);
}

auto Testbench::RunTests(std::ostream &out) -> bool {
  bool result = true;
  result &= RunAuxTests(out);
  result &= RunAstTests(out);
  result &= RunTypeTests(out);
  result &= RunKernelTests(out);
  result &= RunFrontTests(out);
  result &= RunCoreTests(out);
  return result;
}

auto Testbench::RunAuxTests(std::ostream &out) -> bool {
  bool result = true;
  if (RanTest(Testbench::error)) {
    result &= SetTestResult(Testbench::error, TestError(out));
  }

  if (RanTest(Testbench::outcome)) {
    result &= SetTestResult(Testbench::outcome, TestOutcome(out));
  }

  if (RanTest(Testbench::string_interner)) {
    result &=
        SetTestResult(Testbench::string_interner, TestStringInterner(out));
  }

  if (RanTest(Testbench::symbol_table)) {
    result &= SetTestResult(Testbench::symbol_table, TestSymbolTable(out));
  }

  if (RanTest(Testbench::type_interner)) {
    result &= SetTestResult(Testbench::type_interner, TestTypeInterner(out));
  }

  if (RanTest(Testbench::environment)) {
    result &= SetTestResult(Testbench::environment, TestEnvironment(out));
  }

  if (RanTest(Testbench::unop_codegen)) {
    result &= SetTestResult(Testbench::unop_codegen, TestUnopCodegen(out));
  }

  if (RanTest(Testbench::unop_literal)) {
    result &= SetTestResult(Testbench::unop_literal, TestUnopLiteral(out));
  }

  if (RanTest(Testbench::unop_table)) {
    result &= SetTestResult(Testbench::unop_table, TestUnopTable(out));
  }

  if (RanTest(Testbench::binop_codegen)) {
    result &= SetTestResult(Testbench::binop_codegen, TestBinopCodegen(out));
  }

  if (RanTest(Testbench::binop_literal)) {
    result &= SetTestResult(Testbench::binop_literal, TestBinopLiteral(out));
  }

  if (RanTest(Testbench::binop_table)) {
    result &= SetTestResult(Testbench::binop_table, TestBinopTable(out));
  }

  if (RanTest(Testbench::flags)) {
    result &= SetTestResult(Testbench::flags, TestFlags(out));
  }

  if (RanTest(Testbench::location)) {
    result &= SetTestResult(Testbench::location, TestLocation(out));
  }

  SetTestResult(Testbench::aux, result);
  return result;
}

auto Testbench::RunAstTests(std::ostream &out) -> bool {
  bool result = true;
  if (RanTest(Testbench::nil)) {
    result &= SetTestResult(Testbench::nil, TestAstAndNil(out));
  }

  if (RanTest(Testbench::boolean)) {
    result &= SetTestResult(Testbench::boolean, TestBool(out));
  }

  if (RanTest(Testbench::integer)) {
    result &= SetTestResult(Testbench::integer, TestInt(out));
  }

  if (RanTest(Testbench::variable)) {
    result &= SetTestResult(Testbench::variable, TestVariable(out));
  }

  if (RanTest(Testbench::bind)) {
    result &= SetTestResult(Testbench::bind, TestBind(out));
  }

  if (RanTest(Testbench::binop)) {
    result &= SetTestResult(Testbench::binop, TestBinop(out));
  }

  if (RanTest(Testbench::unop)) {
    result &= SetTestResult(Testbench::unop, TestUnop(out));
  }

  if (RanTest(Testbench::assignment)) {
    result &= SetTestResult(Testbench::assignment, TestAssignment(out));
  }

  if (RanTest(Testbench::block)) {
    result &= SetTestResult(Testbench::block, TestBlock(out));
  }

  if (RanTest(Testbench::function)) {
    result &= SetTestResult(Testbench::function, TestFunction(out));
  }

  if (RanTest(Testbench::application)) {
    result &= SetTestResult(Testbench::application, TestApplication(out));
  }

  if (RanTest(Testbench::array)) {
    result &= SetTestResult(Testbench::array, TestArray(out));
  }

  if (RanTest(Testbench::tuple)) {
    result &= SetTestResult(Testbench::tuple, TestTuple(out));
  }

  if (RanTest(Testbench::conditional)) {
    result &= SetTestResult(Testbench::conditional, TestConditional(out));
  }

  if (RanTest(Testbench::dot)) {
    result &= SetTestResult(Testbench::dot, TestDot(out));
  }

  if (RanTest(Testbench::loop)) {
    result &= SetTestResult(Testbench::loop, TestWhile(out));
  }

  SetTestResult(Testbench::ast, result);
  return result;
}

auto Testbench::RunTypeTests(std::ostream &out) -> bool {
  bool result = true;
  if (RanTest(Testbench::nil_type)) {
    result &= SetTestResult(Testbench::nil_type, TestTypeAndNilType(out));
  }

  if (RanTest(Testbench::integer_type)) {
    result &= SetTestResult(Testbench::integer_type, TestIntType(out));
  }

  if (RanTest(Testbench::boolean_type)) {
    result &= SetTestResult(Testbench::boolean_type, TestBoolType(out));
  }

  if (RanTest(Testbench::function_type)) {
    result &= SetTestResult(Testbench::function_type, TestFunctionType(out));
  }

  if (RanTest(Testbench::array_type)) {
    result &= SetTestResult(Testbench::array_type, TestArrayType(out));
  }

  if (RanTest(Testbench::pointer_type)) {
    result &= SetTestResult(Testbench::pointer_type, TestPointerType(out));
  }

  if (RanTest(Testbench::void_type)) {
    result &= SetTestResult(Testbench::void_type, TestVoidType(out));
  }

  if (RanTest(Testbench::tuple_type)) {
    result &= SetTestResult(Testbench::tuple_type, TestTupleType(out));
  }

  SetTestResult(Testbench::type, result);
  return result;
}

auto Testbench::RunKernelTests(std::ostream &out) -> bool {
  bool result = true;
  if (RanTest(Testbench::unop_primitives)) {
    result &=
        SetTestResult(Testbench::unop_primitives, TestUnopPrimitives(out));
  }

  if (RanTest(Testbench::binop_primitives)) {
    result &=
        SetTestResult(Testbench::binop_primitives, TestBinopPrimitives(out));
  }

  SetTestResult(Testbench::kernel, result);
  return result;
}

auto Testbench::RunFrontTests(std::ostream &out) -> bool {
  bool result = true;
  if (RanTest(Testbench::token)) {
    result &= SetTestResult(Testbench::token, TestToken(out));
  }

  if (RanTest(Testbench::lexer)) {
    result &= SetTestResult(Testbench::lexer, TestLexer(out));
  }

  if (RanTest(Testbench::parser)) {
    result &= SetTestResult(Testbench::parser, TestParser(out));
  }

  SetTestResult(Testbench::front, result);
  return result;
}

auto Testbench::RunCoreTests(std::ostream &out) -> bool {
  bool result = true;
  if (RanTest(Testbench::typecheck)) {
    result &= SetTestResult(Testbench::typecheck, TestFirstPhase(out));
  }

  if (RanTest(Testbench::codegen)) {
    result &= SetTestResult(Testbench::codegen, TestBasics(out));
  }

  SetTestResult(Testbench::core, result);
  return result;
}