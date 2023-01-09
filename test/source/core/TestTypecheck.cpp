#include <sstream>
#include <vector>

#include "Test.h"

#include "support/Common.h"

#include "core/TestTypecheck.h"

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
#include "ast/Typecheck.h"
#include "ast/Unop.h"
#include "ast/Variable.h"
#include "ast/While.h"

// we can reuse the boilerplate functions from TestParser
// here, as we test the exact same things, plus the tests
// for Typecheck

auto TestTypecheck(std::ostream &out) -> bool {
  bool result = true;
  out << "\n-----------------------\n";
  out << "Testing Pink First Phase: \n";

  std::vector<std::string> source_lines = {
      "a := 128;\n", "b := true;\n",       "c := a;\n",
      "d := &a;\n",  "fn zero() { 0; }\n",
  };

  auto [test_text, source_locations] = [&source_lines]() {
    size_t line_number = 0;
    std::string test_text;
    std::vector<pink::Location> source_locations;

    for (auto &line : source_lines) {
      line_number += 1;
      source_locations.emplace_back(line_number, 0, line_number,
                                    line.length() - 1);
      test_text += line;
    }

    return std::make_pair(test_text, source_locations);
  }();

  std::stringstream sstream(std::move(test_text));

  auto options = std::make_shared<pink::CLIOptions>();
  auto env = pink::Environment::NewGlobalEnv(options, &sstream);

  /*
    broadly we want to test that:
          Parser emits correct Ast for each Ast node kind,
          and each Ast emits the right type after being returned.

          Parser successfully emits errors when it encounters
          invalid syntax.

          Typecheck successfully emits errors when it encounters
          untypeable expressions.
  */
  pink::Ast *term = nullptr;
  pink::Outcome<pink::Type *, pink::Error> getype;
  pink::Outcome<std::unique_ptr<pink::Ast>, pink::Error> outcome;
  size_t line_number = 0;

  pink::Location loc(source_locations[line_number]);
  outcome = env->parser->Parse(*env);

  auto next_test = [&]() {
    line_number++;
    outcome = env->parser->Parse(*env);
    loc = source_locations[line_number];
  };

  result &= Test(out, "Parser::Parse(a := 128;), Typecheck() == Int",
                 ((term = GetAstOrNull(outcome, out)) != nullptr) &&
                     (TestBindTermIsA<pink::Integer>(term, loc, out)) &&
                     (TestGetypeIsA<pink::IntegerType>(
                         getype = term->Typecheck(*env), out)));

  next_test();
  result &= Test(out, "Parser::Parse(b := true;), Typecheck() == Bool",
                 ((term = GetAstOrNull(outcome, out)) != nullptr) &&
                     (TestBindTermIsA<pink::Boolean>(term, loc, out)) &&
                     (TestGetypeIsA<pink::BooleanType>(
                         getype = term->Typecheck(*env), out)));

  // even though we have not generated the code binding
  // x to a literal 0, Typecheck will have generated a false
  // binding when typing the bind expression, which is normally
  // cleaned up before codegeneration, but here we are only
  // testing Typecheck itself. which is why we can validly type
  // the expression 'c := a'
  next_test();
  result &= Test(out, "Parser::Parse(c := a;), Typecheck() == Integer",
                 ((term = GetAstOrNull(outcome, out)) != nullptr) &&
                     (TestBindTermIsA<pink::Variable>(term, loc, out)) &&
                     (TestGetypeIsA<pink::IntegerType>(
                         getype = term->Typecheck(*env), out)));

  next_test();
  result &= Test(out, "Parser::Parse(d := &a;), Typecheck() == Ptr<Integer>",
                 ((term = GetAstOrNull(outcome, out)) != nullptr) &&
                     (TestBindTermIsA<pink::Unop>(term, loc, out)) &&
                     (TestGetypeIsA<pink::PointerType>(
                         getype = term->Typecheck(*env), out)));

  next_test();
  result &= Test(
      out, "Parser::Parse(fn zero() { 0; }), Typecheck() == Nil -> Integer",
      ((term = GetAstOrNull(outcome, out)) != nullptr) &&
          (TestFnBodyIsA<pink::Integer>(term, loc, out)) &&
          (TestGetypeIsA<pink::FunctionType>(getype = term->Typecheck(*env),
                                             out)));

  result &= Test(out, "pink First Phase", result);

  out << "\n-----------------------\n";
  return result;
}
