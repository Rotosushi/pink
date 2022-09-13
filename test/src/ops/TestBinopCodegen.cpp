
#include "Test.h"
#include "ops/TestBinopCodegen.h"
#include "ops/BinopCodegen.h"

#include "aux/Environment.h"

#include "type/IntType.h"

pink::Outcome<llvm::Value*, pink::Error> test_binop_codegen_fn(llvm::Type* lty, llvm::Value* left, llvm::Type* rty, llvm::Value* right, std::shared_ptr<pink::Environment> env)
{
    pink::Error err(pink::Error::Code::None, pink::Location());
    if (!lty || !left || !rty || !right || !env) // suppress warnings about unused variables.
      return pink::Outcome<llvm::Value*, pink::Error>(err);
    else
      return pink::Outcome<llvm::Value*, pink::Error>(err);
}



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
  
  pink::Outcome<llvm::Value*, pink::Error> gen = binop_codegen.generate(nullptr, nullptr, nullptr, nullptr, env);

  result &= Test(out, "BinopCodegen::generate", binop_codegen.generate == test_binop_codegen_fn && !gen);


  result &= Test(out, "pink::BinopCodegen", result);
  out << "\n-----------------------\n";
  return result;
}
