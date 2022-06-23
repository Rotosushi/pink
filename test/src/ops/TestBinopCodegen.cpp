
#include "Test.h"
#include "ops/TestBinopCodegen.h"
#include "ops/BinopCodegen.h"

#include "aux/Environment.h"

#include "type/IntType.h"

pink::Outcome<llvm::Value*, pink::Error> test_binop_codegen_fn(llvm::Value* left, llvm::Value* right, std::shared_ptr<pink::Environment> env)
{
    std::string s("");
    pink::Error err(pink::Error::Kind::Syntax, s, pink::Location());
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
  
  pink::Outcome<llvm::Value*, pink::Error> gen = binop_codegen.generate(nullptr, nullptr, env);

  result &= Test(out, "BinopCodegen::generate", binop_codegen.generate == test_binop_codegen_fn && !gen);


  result &= Test(out, "pink::BinopCodegen", result);
  out << "\n-----------------------\n";
  return result;
}
