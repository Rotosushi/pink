#include "Test.h"
#include "aux/TestEnvironment.h"
#include "aux/Environment.h"
#include "aux/Error.h"

#include "ast/Nil.h"

bool TestEnvironment(std::ostream& out)
{
    bool result = true;
    out << "\n-----------------------\n";
    out << "Testing Pink::Environment: \n";

    auto options = std::make_shared<pink::CLIOptions>();
    auto env = pink::NewGlobalEnv(options);

    pink::InternedString symb = env->symbols->Intern("x");

    result &= Test(out, "Environment::symbols", symb == std::string("x"));


    pink::InternedString op = env->operators->Intern("+");

    result &= Test(out, "Environment::operators", op == std::string("+"));


    pink::Type* type = env->types->GetNilType();

    result &= Test(out, "Environment::types", type->ToString() == std::string("Nil"));

    pink::Location l(0, 7, 0, 10);
    llvm::Value* nil = env->instruction_builder->getFalse();

    env->bindings->Bind(symb, type, nil);

    llvm::Optional<std::pair<pink::Type*, llvm::Value*>> term = env->bindings->Lookup(symb);

    // since they point to the same memory, nil, and the bound
    // term's pointer values compare equal if everything works.
    result &= Test(out, "Environment::bindings", term.has_value() && (*term).first == type && (*term).second == nil);

    // #TODO: write more env tests. 


    result &= Test(out, "pink::Environment", result);

    out << "\n-----------------------\n";
    return result;
}
