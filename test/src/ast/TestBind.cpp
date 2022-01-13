#include "Test.hpp"
#include "ast/TestBind.hpp"

#include "ast/Nil.hpp"
#include "ast/Bind.hpp"

bool TestBind(std::ostream& out)
{
    bool result = true;
    out << "\n-----------------------\n";
    out << "Testing pink::Bind: \n";

    pink::StringInterner s;
    pink::InternedString v = s.Intern("v");
    pink::Location l0(0, 0, 0, 5);
    pink::Location l1(0, 6, 0, 9);
    pink::Nil*  n0 = new pink::Nil(l1);
    pink::Bind* b0 = new pink::Bind(l0, v, n0);

    /*
    The Ast class itself only provides a small
    amount of the functionality of any given
    syntax node.

    tests:

    -) An Ast node constructed with a particular kind
        holds that particular kind.

    -) classof() meets specifications:
        https://llvm.org/docs/HowToSetUpLLVMStyleRTTI.html#the-contract-of-classof

    -) An Ast node constructed with a particular Location
        holds that particular Location.

    -) Bind::symbol == symbol;

    -) Bind::term != nullptr;

    -) Bind::ToString() == std::string(symbol) + " := " + term->ToString();

    */

    result &= Test(out, "Bind::GetKind()", b0->getKind() == pink::Ast::Kind::Bind);
    result &= Test(out, "Bind::classof()", b0->classof(b0));

    pink::Location l2(l0);
    pink::Location bl(b0->GetLoc());
    result &= Test(out, "Bind::GetLoc()", bl == l2);

    result &= Test(out, "Bind::symbol", b0->symbol == v);
    result &= Test(out, "Bind::term", b0->term == n0);

    std::string bind_str = std::string(v) + std::string(" := ") + n0->ToString();

    result &= Test(out, "Bind::ToString()", b0->ToString() == bind_str);

    delete b0;

    result &= Test(out, "pink::Bind", result);
    out << "\n-----------------------\n";
    return result;
}
