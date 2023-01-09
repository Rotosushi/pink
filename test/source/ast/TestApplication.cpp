#include <sstream>

#include "Test.h"
#include "ast/Application.h"
#include "ast/Boolean.h"
#include "ast/Function.h"
#include "ast/Integer.h"
#include "ast/TestApplication.h"
#include "ast/Variable.h"

#include "aux/Environment.h"

auto TestApplication(std::ostream &out) -> bool {
  bool result = true;
  out << "\n-----------------------------\n";
  out << "Testing pink::Application: \n";

  auto options = std::make_shared<pink::CLIOptions>();
  std::stringstream stream;
  stream.str("fn add(x: Int, y: Int) { x + y }");

  auto env = pink::Environment::NewGlobalEnv(options, &stream);

  auto fun = env->parser->Parse(*env);

  // "add(5, 7)"
  // the only algorithmic way to count out the locations properly
  // is to actually lex the input string just for the locations.
  // given that we are only testing that we can retrieve the passed
  // in locations, it makes sense to use some admittedly magic numbers here.
  // (think about it, the only way to get the computer to really check that
  //  the locations emitted by the lexer are correct is to write another lexer,
  //  and be sure of that lexer's ability to check the first. which begs
  //  the question of how do we test that lexer, and so on. eventually there
  //  has to be a magic number somewhere when we stop, so it might as well be
  //  here.)
  pink::Location idloc(2, 0, 2, 3);   // NOLINT
  pink::Location arg0loc(2, 5, 2, 6); // NOLINT
  pink::Location arg1loc(2, 8, 2, 9); // NOLINT
  auto fnname =
      std::make_unique<pink::Variable>(idloc, env->symbols->Intern("add"));

  std::vector<std::unique_ptr<pink::Ast>> args;

  args.emplace_back(std::make_unique<pink::Integer>(arg0loc, 5)); // NOLINT
  args.emplace_back(std::make_unique<pink::Integer>(arg1loc, 7)); // NOLINT

  pink::Location apploc(1, 0, 1, 7); // NOLINT
  auto app = std::make_unique<pink::Application>(apploc, std::move(fnname),
                                                 std::move(args));

  result &= Test(out, "Application::getKind()",
                 app->GetKind() == pink::Ast::Kind::Application);

  result &= Test(out, "Application::classof()", app->classof(app.get()));

  result &= Test(out, "Application::GetLoc()", app->GetLoc() == apploc);

  std::string appstr = "add(5, 7)";
  result &= Test(out, "Application::ToString()", app->ToString() == appstr);

  // so far, Application::Typecheck will fail because we do not bind function
  // definitions to llvm::Value*'s which we can never do until we codegen
  // the function definition. now, this is a very interesting question,
  // because when are we supposed to codegen function definitions when trying
  // to type a newly parsed block of statements?
  //
  //
  //

  // we must add a function definition to be applied, so Application::Typecheck
  // can actually work against the function definition.

  pink::Type *int_type = env->types->GetIntType();

  std::vector<pink::Type *> arg_types = {int_type, int_type};
  pink::Type *fn_type = env->types->GetFunctionType(int_type, arg_types);

  env->bindings->Bind(env->symbols->Intern("add"), fn_type,
                      /* llvm::Value* term = */ nullptr);

  auto typecheck_result = app->Typecheck(*env);
  result &= Test(out, "Application::Typecheck()",
                 typecheck_result && typecheck_result.GetFirst() == int_type);

  // given an application of not a function,
  // do we get an error?

  // given an application of a function given the
  // wrong number of arguments,
  // do we get an error?

  // given an application of a function given the
  // wrong type of arguments,
  // do we get an error?

  return Test(out, "pink::Application", result);
}
