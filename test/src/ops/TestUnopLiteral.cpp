#include "Test.h"
#include "ops/TestUnopLiteral.h"
#include "ops/UnopLiteral.h"

#include "aux/Environment.h"

#include "type/IntType.h"

pink::Outcome<llvm::Value*, pink::Error> test_literal_fn(llvm::Value* term, std::shared_ptr<pink::Environment> env)
{
    std::string s("");
    pink::Error err(pink::Error::Kind::Syntax, s, pink::Location());
    return pink::Outcome<llvm::Value*, pink::Error>(err);
}

bool TestUnopLiteral(std::ostream& out)
{
    bool result = true;
    out << "\n-----------------------\n";
    out << "Testing pink::UnopLiteral: \n";
    
    auto options = std::make_shared<pink::CLIOptions>();
    auto env     = pink::NewGlobalEnv(options);
      
    pink::Type* ty = env->types->GetIntType();
    pink::UnopLiteral unop;

    auto pair = unop.Register(ty, ty, test_literal_fn);

    result &= Test(out, "UnopLiteral::Register()", pair.first == ty);


    auto opt = unop.Lookup(ty);

    result &= Test(out, "UnopLiteral::Lookup()", opt.hasValue() && (*opt).first == ty);

    pink::Outcome<llvm::Value*, pink::Error> v = (*opt).second->generate(nullptr, env);

    result &= Test(out, "UnopLiteral::generate", !v.GetWhich());

    unop.Unregister(ty);

    opt = unop.Lookup(ty);

    result &= Test(out, "UnopLiteral::Unregister()", !opt.hasValue());


    result &= Test(out, "pink::UnopLiteral", result);
    out << "\n-----------------------\n";
    return result;
}
