#include "ops/TestUnopCodegen.h"
#include "Test.h"
#include "ops/UnopCodegen.h"

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
    pink::Outcome<llvm::Value *, pink::Error> test_codegen_fn(
        llvm::Value *term, const pink::Environment &env) {
      return {pink::Error()};
    })

bool TestUnopCodegen(std::ostream &out) {
  bool result = true;
  out << "\n-----------------------\n";
  out << "Testing pink::UnopCodegen: \n";

  auto options = std::make_shared<pink::CLIOptions>();
  auto env = pink::Environment::NewGlobalEnv(options);

  pink::Type *ty = env->types->GetIntType();
  pink::UnopCodegen unop_gen(ty, test_codegen_fn);

  result &= Test(out, "UnopCodegen::result_type", unop_gen.result_type == ty);

  pink::Outcome<llvm::Value *, pink::Error> v =
      unop_gen.generate(nullptr, *env);

  result &= Test(out, "UnopCodegen::generate",
                 unop_gen.generate == test_codegen_fn && !v);

  result &= Test(out, "pink::UnopCodegen", result);
  out << "\n-----------------------\n";
  return result;
}
