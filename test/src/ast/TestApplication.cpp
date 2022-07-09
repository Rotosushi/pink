
#include "Test.h"
#include "ast/TestApplication.h"
#include "ast/Application.h"
#include "ast/Function.h"
#include "ast/Int.h"
#include "ast/Bool.h"

#include "aux/Environment.h"

bool TestApplication(std::ostream& out)
{
  bool result = true;
  out << "\n-----------------------------\n";
  out << "Testing pink::Application: \n";

  auto options = std::make_shared<pink::CLIOptions>();
  auto env     = pink::NewGlobalEnv(options);

  // we need to define a function and then construct an application term
  // which calls the function.

  pink::Outcome<std::unique_ptr<pink::Ast>, pink::Error> fn = env->parser->Parse("fn add(x: Int, y: Int) { x + y }", env);

  pink::Outcome<std::unique_ptr<pink::Ast>, pink::Error> fnname = env->parser->Parse("add", env);
  
  std::vector<std::unique_ptr<pink::Ast>> args;

  args.emplace_back(std::move(env->parser->Parse("5", env).GetOne()));
  args.emplace_back(std::move(env->parser->Parse("7", env).GetOne()));

  pink::Location apploc;
  std::unique_ptr<pink::Application> app(new pink::Application(apploc, std::move(fnname.GetOne()), std::move(args)));

  // given an application term can we access the peripheral data?
  result &= Test(out, "Application::getKind()", app->getKind() == pink::Ast::Kind::Application);

  result &= Test(out, "Application::classof()", app->classof(app.get()));

  result &= Test(out, "Application::GetLoc()", app->GetLoc() == apploc);

  // pink::Application::ToString() prints
  // "add;\n 5;\n 7;\n"
  // because the ToString methods are adding to much to each print.
  std::string appstr = "add 5 7";
  result &= Test(out, "Application::ToString()", app->ToString() == appstr);

  // given an application of a defined function, 
  // is the type correct?

  pink::Type* int_type = env->types->GetIntType();
  pink::Outcome<pink::Type*, pink::Error> app_type = app->Getype(env);
  result &= Test(out, "Application::Getype()", app_type && app_type.GetOne() == int_type);

  // given an application of not a function,
  // do we get an error?
  
  // given an application of a function given the 
  // wrong number of arguments,
  // do we get an error?
  
  // given an application of a function given the 
  // wrong type of arguments,
  // do we get an error?


  result &= Test(out, "pink::Application", result);
  return result;
}
