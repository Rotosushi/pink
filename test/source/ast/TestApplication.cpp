#include <sstream>

#include "Test.h"
#include "ast/Application.h"
#include "ast/Boolean.h"
#include "ast/Function.h"
#include "ast/Integer.h"
#include "ast/TestApplication.h"
#include "ast/Variable.h"

#include "ast/action/ToString.h"
#include "ast/action/Typecheck.h"

#include "aux/Environment.h"

auto TestApplication(std::ostream &out) -> bool {
  bool        result = true;
  std::string name   = "pink::Application";
  TestHeader(out, name);

  auto              options = std::make_shared<pink::CLIOptions>();
  std::stringstream stream;
  stream.str("fn add(x: Int, y: Int) { x + y }");

  auto env = pink::Environment::NewGlobalEnv(options, &stream);

  auto fun = env->parser->Parse(*env);

  // so, after more thought, we should not test ToString or
  // Typecheck in any of the Ast Test routines.
  //
  // ToString should be tested alongside TestParser
  // so as to test that they are inverses of one another.
  // and to check that the location's parsed are more
  // in line with what is present in the input text.
  //
  // Typecheck should be tested in it's own routine.
  // so as to check all success states and fail states.
  // And after the parser, such that we can utilize the
  // parser to construct the terms to be typed.
  pink::Location idloc(2, 0, 2, 3);   // NOLINT
  pink::Location arg0loc(2, 5, 2, 6); // NOLINT
  pink::Location arg1loc(2, 8, 2, 9); // NOLINT
  auto           fnname =
      std::make_unique<pink::Variable>(idloc, env->symbols->Intern("add"));

  std::vector<std::unique_ptr<pink::Ast>> args;

  args.emplace_back(std::make_unique<pink::Integer>(arg0loc, 5)); // NOLINT
  args.emplace_back(std::make_unique<pink::Integer>(arg1loc, 7)); // NOLINT

  pink::Location     apploc(1, 0, 1, 7); // NOLINT
  pink::Ast::Pointer app = std::make_unique<pink::Application>(
      apploc, std::move(fnname), std::move(args));

  result &= Test(out, "Application::getKind()",
                 app->GetKind() == pink::Ast::Kind::Application);

  result &=
      Test(out, "Application::classof()", llvm::isa<pink::Application>(app));

  result &= Test(out, "Application::GetLoc()", app->GetLocation() == apploc);

  std::string appstr = "add(5, 7)";
  result &= Test(out, "Application::ToString()", ToString(app) == appstr);

  pink::Type::Pointer int_type = env->types->GetIntType();

  std::vector<pink::Type::Pointer> arg_types = {int_type, int_type};
  pink::Type::Pointer              fn_type =
      env->types->GetFunctionType(int_type, arg_types);

  env->bindings->Bind(env->symbols->Intern("add"), fn_type, nullptr);

  auto typecheck_result = Typecheck(app, *env);
  result                &= Test(out, "Application::Typecheck()",
                                typecheck_result && typecheck_result.GetFirst() == int_type);

  return TestFooter(out, name, result);
}
