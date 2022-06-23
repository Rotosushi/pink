#include "Test.h"

#include "kernel/TestUnopPrimitives.h"
#include "kernel/UnopPrimitives.h"


bool TestUnopPrimitives(std::ostream& out)
{
    bool result = true;
    out << "\n-----------------------\n";
    out << "Testing Pink::UnopPrimitives: \n";

    auto options = std::make_shared<pink::CLIOptions>();
    auto env     = pink::NewGlobalEnv(options);

    pink::InternedString minus_str = env->operators->Intern("-");
    pink::InternedString bang_str  = env->operators->Intern("!");

    auto opt0 = env->unops->Lookup(minus_str);

    result &= Test(out, "UnopPrimitive::IntNegation", opt0.hasValue() && (*opt0).first == minus_str);

    auto opt1 = env->unops->Lookup(bang_str);

    result &= Test(out, "UnopPrimitive::BoolNegation", opt1.hasValue() && (*opt1).first == bang_str);


    result &= Test(out, "pink::UnopPrimitives", result);
    out << "\n-----------------------\n";
    return result;
}
