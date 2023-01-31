
#include "ops/TestBinopCodegen.h"
#include "Test.h"
#include "ops/BinopCodegen.h"

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
    llvm::Value *test_binop_codegen_fn(llvm::Type *lty, llvm::Value *left,
                                       llvm::Type *rty, llvm::Value *right,
                                       const pink::Environment &env) {
      return nullptr;
    })

bool TestBinopCodegen(std::ostream &out) {
  bool        result = true;
  std::string name   = "pink::BinopCodegen";
  TestHeader(out, name);

  auto options = std::make_shared<pink::CLIOptions>();
  auto env     = pink::Environment::NewGlobalEnv(options);

  pink::Type::Pointer ty = env->types->GetIntType();
  pink::BinopCodegen  binop_codegen(ty, test_binop_codegen_fn);

  result &=
      Test(out, "BinopCodegen::result_type", binop_codegen.result_type == ty);

  result &= Test(out, "BinopCodegen::generate",
                 binop_codegen.generate == test_binop_codegen_fn);

  return TestFooter(out, name, result);
}
