
#include "Test.h"
#include "ops/TestBinopCodegen.h"
#include "ops/BinopCodegen.h"

#include "aux/Environment.h"

#include "type/IntType.h"

#include "support/DisableWarning.h"

// NOTE: 9/15/2022
// This function is a stub which is only defined for the sake of the 
// test within this file, and is not expected to have -any- functionality.
// as such we do not use all of the parameters. since this is expected,
// we are choosing to disable this specific warning just for the function.
NOWARN(-Wunused-parameter, 
pink::Outcome<llvm::Value*, pink::Error> test_binop_codegen_fn(llvm::Type* lty, llvm::Value* left, llvm::Type* rty, llvm::Value* right, const pink::Environment& env)
{
    pink::Error err(pink::Error::Code::None, pink::Location());
    return pink::Outcome<llvm::Value*, pink::Error>(err);
}
)


bool TestBinopCodegen(std::ostream& out)
{
  bool result = true;
  out << "\n-----------------------\n";
  out << "Testing pink::BinopCodegen: \n";
  
  auto options = std::make_shared<pink::CLIOptions>();
  auto env = pink::NewGlobalEnv(options);

  pink::Type* ty = env->types->GetIntType();
  pink::BinopCodegen binop_codegen(ty, test_binop_codegen_fn);

  result &= Test(out, "BinopCodegen::result_type", binop_codegen.result_type == ty);
  
  pink::Outcome<llvm::Value*, pink::Error> gen = binop_codegen.generate(nullptr, nullptr, nullptr, nullptr, *env);

  result &= Test(out, "BinopCodegen::generate", binop_codegen.generate == test_binop_codegen_fn && !gen);


  result &= Test(out, "pink::BinopCodegen", result);
  out << "\n-----------------------\n";
  return result;
}
