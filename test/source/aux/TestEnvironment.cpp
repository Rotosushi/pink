#include "aux/TestEnvironment.h"
#include "Test.h"
#include "aux/Environment.h"
#include "aux/Error.h"

#include "ast/Nil.h"

#include "ast/action/ToString.h"
#include "ast/action/Typecheck.h"

#include "type/action/ToString.h"

auto TestEnvironment(std::ostream &out) -> bool {
  bool        result = true;
  std::string name   = "pink::Environment";
  TestHeader(out, name);

  auto options = std::make_shared<pink::CLIOptions>();
  auto env     = pink::Environment::NewGlobalEnv(options);

  pink::InternedString symb = env->symbols->Intern("x");

  result &= Test(out, "Environment::symbols", symb == std::string("x"));

  pink::InternedString opr = env->operators->Intern("+");

  result &= Test(out, "Environment::operators", opr == std::string("+"));

  pink::Type::Pointer type = env->types->GetNilType();

  result &=
      Test(out, "Environment::types", ToString(type) == std::string("Nil"));

  llvm::Value *nil = env->instruction_builder->getFalse();

  env->bindings->Bind(symb, type, nil);

  auto term = env->bindings->Lookup(symb);

  result &=
      Test(out, "Environment::bindings",
           term.has_value() && (*term).first == type && (*term).second == nil);

  // #TODO: write more env tests.

  result &= Test(out, "pink::Environment", result);

  return TestFooter(out, name, result);
}
