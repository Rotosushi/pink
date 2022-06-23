#include "Test.h"
#include "ops/TestBinopLiteral.h"
#include "ops/BinopLiteral.h"

#include "aux/Environment.h"

pink::Outcome<llvm::Value*, pink::Error> test_binop_literal_fn(llvm::Value* left, llvm::Value* right, std::shared_ptr<pink::Environment> env)
{
    std::string s("");
    pink::Error err(pink::Error::Kind::Syntax, s, pink::Location());
    return pink::Outcome<llvm::Value*, pink::Error>(err);
}

bool TestBinopLiteral(std::ostream& out)
{
    bool result = true;
    out << "\n-----------------------\n";
    out << "Testing pink::BinopLiteral: \n";
    
    auto options = std::make_shared<pink::CLIOptions>();
    auto env     = pink::NewGlobalEnv(options);

	  pink::Type* ty = env->types->GetIntType();
    pink::Precedence p = 5;
    pink::Associativity a = pink::Associativity::Left;
    pink::BinopLiteral binop(p, a);

    result &= Test(out, "BinopLiteral::precedence", binop.precedence == p);
    result &= Test(out, "BinopLiteral::associativity", binop.associativity == a);

    auto pair = binop.Register(ty, ty, ty, test_binop_literal_fn);

    result &= Test(out, "BinopLiteral::Register()", pair.first == std::make_pair(ty, ty));

    auto opt = binop.Lookup(ty, ty);

    result &= Test(out, "BinopLiteral::Lookup()", opt.hasValue() && (*opt).first == std::make_pair(ty, ty));

    binop.Unregister(ty, ty);

    opt = binop.Lookup(ty, ty);

    result &= Test(out, "BinopLiteral::Unregister()", !opt.hasValue());

    result &= Test(out, "pink::BinopLiteral", result);
    out << "\n-----------------------\n";
    return result;
}
