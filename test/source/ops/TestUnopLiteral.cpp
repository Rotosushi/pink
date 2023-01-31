#include "ops/TestUnopLiteral.h"
#include "Test.h"
#include "ops/UnopLiteral.h"

#include "aux/Environment.h"

#include "type/IntType.h"

#include "support/DisableWarning.h"

// NOTE: 9/15/2022
// This function is a stub which is only defined for the sake of the
// test within this file, and is not expected to have -any- functionality.
// as such we do not use all of the parameters. since this is expected,
// we are choosing to disable this specific warning just for the function.
NOWARN(
    "-Wunused-parameter",
    llvm::Value *test_literal_fn(llvm::Value             *term,
                                 const pink::Environment &env) {
      return nullptr;
    })

bool TestUnopLiteral(std::ostream &out) {
  bool        result = true;
  std::string name   = "pink::UnopLiteral";
  TestHeader(out, name);

  auto options = std::make_shared<pink::CLIOptions>();
  auto env     = pink::Environment::NewGlobalEnv(options);

  pink::Type::Pointer ty = env->types->GetIntType();
  pink::UnopLiteral   unop;

  auto pair = unop.Register(ty, ty, test_literal_fn);

  result &= Test(out, "UnopLiteral::Register()", pair.first == ty);

  auto opt = unop.Lookup(ty);

  result &= Test(out, "UnopLiteral::Lookup()",
                 opt.has_value() && opt->second->GetReturnType() == ty);

  result &= Test(out, "UnopLiteral::generate",
                 opt->second->GetGenerateFn() == test_literal_fn);

  unop.Unregister(ty);

  opt = unop.Lookup(ty);

  result &= Test(out, "UnopLiteral::Unregister()", !opt.has_value());

  return TestFooter(out, name, result);
}
