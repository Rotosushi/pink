#include "Test.h"
#include "ops/TestUnopCodegen.h"
#include "ops/UnopCodegen.h"

#include "aux/Environment.h"

#include "type/IntType.h"

pink::Outcome<llvm::Value*, pink::Error> test_codegen_fn(llvm::Value* term, std::shared_ptr<pink::Environment> env)
{
    pink::Error err(pink::Error::Code::None, pink::Location());
    if (!term || !env)
      return pink::Outcome<llvm::Value*, pink::Error>(err);
    else 
      return pink::Outcome<llvm::Value*, pink::Error>(err);
}

bool TestUnopCodegen(std::ostream& out)
{
    bool result = true;
    out << "\n-----------------------\n";
    out << "Testing pink::UnopCodegen: \n";

    auto options = std::make_shared<pink::CLIOptions>();
    auto env     = pink::NewGlobalEnv(options);

    pink::Type* ty = env->types->GetIntType();
    pink::UnopCodegen unop_gen(ty, test_codegen_fn);

    result &= Test(out, "UnopCodegen::result_type", unop_gen.result_type == ty);


    pink::Outcome<llvm::Value*, pink::Error> v = unop_gen.generate(nullptr, env);

    result &= Test(out, "UnopCodegen::generate", unop_gen.generate == test_codegen_fn && !v);

    result &= Test(out, "pink::UnopCodegen", result);
    out << "\n-----------------------\n";
    return result;
}
