#include <sstream>

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
#include "ast/Bool.h"
#include "ast/Conditional.h"
#include "ast/Dot.h"
#include "ast/Function.h"
#include "ast/Int.h"
#include "ast/Nil.h"
#include "ast/Tuple.h"
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

  std::stringstream ss;
  ss.str(std::string("var x := 0;\n") + std::string("var y := true;\n") +
         std::string("var z := x;\n") + std::string("fn zero() { 0; }\n"));
  auto options = std::make_shared<pink::CLIOptions>();
  auto env = pink::NewGlobalEnv(options, &ss);

  /*
          Parser emits correct Ast for each Ast node kind,
          and each Ast emits the right type after being returned.

          Parser successfully emits errors when it encounters
          invalid syntax.

          Typecheck successfully emits errors when it encounters
          untypeable expressions.
  */
  pink::Ast *term = nullptr;
  pink::Outcome<pink::Type *, pink::Error> getype = pink::Error();
  pink::Outcome<std::unique_ptr<pink::Ast>, pink::Error> outcome =
      pink::Error();
  size_t ln = 1;
  pink::Location loc(ln, 0, ln, 11);

  outcome = env->parser->Parse(*env);

  result &=
      Test(out, "Parser::Parse(var x := 0;), Typecheck() == Int",
           ((term = GetAstOrNull(outcome)) != nullptr) &&
               (TestBindTermIsA<pink::Int>(term, loc)) &&
               (TestGetypeIsA<pink::IntType>(getype = term->Typecheck(*env))));

  outcome = env->parser->Parse(*env);
  ln++;
  loc = {ln, 0, ln, 14};

  result &=
      Test(out, "Parser::Parse(var y := true;), Typecheck() == Bool",
           ((term = GetAstOrNull(outcome)) != nullptr) &&
               (TestBindTermIsA<pink::Bool>(term, loc)) &&
               (TestGetypeIsA<pink::BoolType>(getype = term->Typecheck(*env))));

  // even though we have not generated the code binding
  // x to a literal 0, Typecheck will have generated a false
  // binding when typing the bind expression, which is normally
  // cleaned up before codegeneration, but here we are only
  // testing Typecheck itself.
  outcome = env->parser->Parse(*env);
  ln++;
  loc = {ln, 0, ln, 11};

  result &=
      Test(out, "Parser::Parse(var z := x;), Typecheck() == Int",
           ((term = GetAstOrNull(outcome)) != nullptr) &&
               (TestBindTermIsA<pink::Variable>(term, loc)) &&
               (TestGetypeIsA<pink::IntType>(getype = term->Typecheck(*env))));

  outcome = env->parser->Parse(*env);
  ln++;
  loc = {ln, 0, ln, 16};

  result &= Test(
      out, "Parser::Parse(fn zero() { 0; }), Typecheck() == Nil -> Int",
      ((term = GetAstOrNull(outcome)) != nullptr) &&
          (TestFnBodyIsA<pink::Int>(term, loc)) &&
          (TestGetypeIsA<pink::FunctionType>(getype = term->Typecheck(*env))));

  result &= Test(out, "pink First Phase", result);

  out << "\n-----------------------\n";
  return result;
}
