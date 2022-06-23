#include "Test.h"
#include "kernel/TestBinopPrimitives.h"
#include "kernel/BinopPrimitives.h"

#include "aux/Environment.h"

bool TestBinopPrimitives(std::ostream& out)
{
    bool result = true;
    out << "\n-----------------------\n";
    out << "Testing Pink::BinopPrimitives: \n";

    auto options = std::make_shared<pink::CLIOptions>();
    auto env     = pink::NewGlobalEnv(options);

    pink::InternedString minus = env->operators->Intern("-");
    pink::InternedString plus  = env->operators->Intern("+");
    pink::InternedString mul   = env->operators->Intern("*");
    pink::InternedString div   = env->operators->Intern("\\");
    pink::InternedString mod   = env->operators->Intern("%");
    pink::InternedString land  = env->operators->Intern("&&");
    pink::InternedString lor   = env->operators->Intern("||");
    pink::InternedString cmpeq = env->operators->Intern("==");

    auto opt0 = env->binops->Lookup(minus);

    result &= Test(out, "BinopPrimitive::IntSub", opt0.hasValue() && (*opt0).first == minus);

    auto opt1 = env->binops->Lookup(plus);

    result &= Test(out, "BinopPrimitive::IntAdd", opt1.hasValue() && (*opt1).first == plus);

    auto opt2 = env->binops->Lookup(mul);

    result &= Test(out, "BinopPrimitive::IntMul", opt2.hasValue() && (*opt2).first == mul);

    auto opt3 = env->binops->Lookup(div);

    result &= Test(out, "BinopPrimitive::IntDiv", opt3.hasValue() && (*opt3).first == div);

    auto opt4 = env->binops->Lookup(mod);

    result &= Test(out, "BinopPrimitive::IntMod", opt4.hasValue() && (*opt4).first == mod);

    auto opt5 = env->binops->Lookup(land);

    result &= Test(out, "BinopPrimitive::BoolAnd", opt5.hasValue() && (*opt5).first == land);

    auto opt6 = env->binops->Lookup(lor);

    result &= Test(out, "BinopPrimitive::BoolOr", opt6.hasValue() && (*opt6).first == lor);

    auto opt7 = env->binops->Lookup(cmpeq);

    result &= Test(out, "BinopPrimitive::Eq", opt7.hasValue() && (*opt7).first == cmpeq);

    result &= Test(out, "pink::BinopPrimitives", result);
    out << "\n-----------------------\n";
    return result;
}
