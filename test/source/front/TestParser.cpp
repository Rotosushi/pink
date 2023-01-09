#include <numeric> // std::accumulate
#include <sstream>
#include <vector>

#include "Test.h"
#include "support/Common.h"

#include "front/Parser.h"
#include "front/TestParser.h"

#include "aux/Environment.h"

#include "ast/Application.h"
#include "ast/Array.h"
#include "ast/Assignment.h"
#include "ast/Bind.h"
#include "ast/Binop.h"
#include "ast/Block.h"
#include "ast/Boolean.h"
#include "ast/Conditional.h"
#include "ast/Dot.h"
#include "ast/Function.h"
#include "ast/Integer.h"
#include "ast/Nil.h"
#include "ast/Tuple.h"
#include "ast/Unop.h"
#include "ast/Variable.h"
#include "ast/While.h"

auto TestParser(std::ostream &out) -> bool {
  bool result = true;
  out << "\n-----------------------\n";
  out << "Testing Pink::Parser: \n";

  // abstract out function testing common code into
  // templated routines. a-la 'TestBindTermIsA<T>'

  std::vector<std::string> source_lines = {
      "w := 0;\n",                   // bind to basic(integer)
      "x := true;\n",                // bind to basic(boolean)
      "y := nil;\n",                 // bind to basic(nil)
      "z := x;\n",                   // bind to basic(variable)
      "a := -1;\n",                  // bind to basic(unop)
      "b := 1 + 1;\n",               // bind to infix
      "c := [5, 6, 7, 8, 9];\n",     // bind to array literal
      "d := (true, true, false);\n", // bind to tuple literal
      "e := (false);\n",             // bind to parenthesized expression
      "f := a = -2;\n",              // bind to assignment
      "fn zero() { 0; }\n",   // function with no arguments, integer return type
      "fn noop() { nil; }\n", // function with no arguments, no return type
      "fn inc(x: Int) { x + 1; }\n", // function with one argument, integer
                                     // return type
      "fn assign() { x = 4; }\n", // function with no arguments, integer return
                                  // type
      "fn add(x : Int, y: Int) { z := x + y; }\n", // function with two
                                                   // arguments, integer return
                                                   // type
  };

  auto [source_locations, teststream] = [&source_lines]() {
    std::vector<pink::Location> source_locations;
    std::string teststring;
    size_t line_number = 0;

    for (auto &line : source_lines) {
      line_number += 1;
      // note it's (line.length() - 1) so we don't count the newline,
      // as the newline is not part of the parsed expression.
      source_locations.emplace_back(line_number, 0, line_number,
                                    line.length() - 1);
      teststring += line;
    }

    return std::make_pair(source_locations, std::stringstream(teststring));
  }();

  auto options = std::make_shared<pink::CLIOptions>();
  auto env = pink::Environment::NewGlobalEnv(options, &teststream);
  auto *parser = env->parser.get();

  size_t line_number = 0;
  auto location = source_locations[line_number];
  auto outcome = parser->Parse(*env);
  pink::Ast *expr = nullptr;

  auto next_test = [&]() {
    line_number++;
    location = source_locations[line_number];
    outcome = parser->Parse(*env);
  };

  result &= Test(out, "Parser::Parse(\"w := 0;\")",
                 ((expr = GetAstOrNull(outcome, out)) != nullptr) &&
                     TestBindTermIsA<pink::Integer>(expr, location, out));

  next_test();
  result &= Test(out, "Parser::Parse(\"x := true;\")",
                 ((expr = GetAstOrNull(outcome, out)) != nullptr) &&
                     TestBindTermIsA<pink::Boolean>(expr, location, out));

  next_test();
  result &= Test(out, "Parser::Parse(\"y := nil\")",
                 ((expr = GetAstOrNull(outcome, out)) != nullptr) &&
                     TestBindTermIsA<pink::Nil>(expr, location, out));

  next_test();
  result &= Test(out, "Parser::Parse(\"z := x\")",
                 ((expr = GetAstOrNull(outcome, out)) != nullptr) &&
                     TestBindTermIsA<pink::Variable>(expr, location, out));

  next_test();
  result &= Test(out, "Parser::Parse(\"a := -1;\")",
                 ((expr = GetAstOrNull(outcome, out)) != nullptr) &&
                     TestBindTermIsA<pink::Unop>(expr, location, out));

  next_test();
  result &= Test(out, "Parser::Parse(\"b := 1 + 1;\")",
                 ((expr = GetAstOrNull(outcome, out)) != nullptr) &&
                     TestBindTermIsA<pink::Binop>(expr, location, out));

  next_test();
  result &= Test(out, "Parser::Parse(\"c := [5, 6, 7, 8, 9];\")",
                 ((expr = GetAstOrNull(outcome, out)) != nullptr) &&
                     TestBindTermIsA<pink::Array>(expr, location, out));

  next_test();
  result &= Test(out, "Parser::Parse(\"d := (true, true, false);\")",
                 ((expr = GetAstOrNull(outcome, out)) != nullptr) &&
                     TestBindTermIsA<pink::Tuple>(expr, location, out));

  next_test();
  result &= Test(out, "Parser::Parse(\"e := (false);\")",
                 ((expr = GetAstOrNull(outcome, out)) != nullptr) &&
                     TestBindTermIsA<pink::Boolean>(expr, location, out));

  next_test();
  result &= Test(out, "Parser::Parse(\"f := a = -2;\")",
                 ((expr = GetAstOrNull(outcome, out)) != nullptr) &&
                     TestBindTermIsA<pink::Assignment>(expr, location, out));

  next_test();
  result &= Test(out, "Parser::Parse(\"fn zero() { 0; }\")",
                 ((expr = GetAstOrNull(outcome, out)) != nullptr) &&
                     TestFnBodyIsA<pink::Integer>(expr, location, out));

  next_test();
  result &= Test(out, "Parser::Parse(\"fn noop() { nil; }\")",
                 ((expr = GetAstOrNull(outcome, out)) != nullptr) &&
                     TestFnBodyIsA<pink::Nil>(expr, location, out));

  next_test();
  result &= Test(out, "Parser::Parse(\"fn inc(x: Int) { x + 1; }\")",
                 ((expr = GetAstOrNull(outcome, out)) != nullptr) &&
                     TestFnBodyIsA<pink::Binop>(expr, location, out));

  next_test();
  result &= Test(out, "Parser::Parse(\"fn assign() { x = 4; }\")",
                 ((expr = GetAstOrNull(outcome, out)) != nullptr) &&
                     TestFnBodyIsA<pink::Assignment>(expr, location, out));

  next_test();
  result &=
      Test(out, "Parser::Parse(\"fn add(x : Int, y: Int) { z := x + y; }\")",
           ((expr = GetAstOrNull(outcome, out)) != nullptr) &&
               TestFnBodyIsA<pink::Bind>(expr, location, out));

  result &= Test(out, "pink::Parser", result);
  out << "\n-----------------------\n";
  return result;
}
