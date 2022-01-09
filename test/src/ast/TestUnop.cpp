#include "Test.hpp"
#include "ast/TestUnop.hpp"
#include "ast/Unop.hpp"
#include "ast/Int.hpp"

bool TestUnop(std::ostream& out)
{
    bool result = true;
    result = true;

    out << "\n-----------------------\n";
    out << "Testing pink::Unop: \n";

    pink::StringInterner s;
    pink::InternedString minus = s.Intern("-");
    pink::Location l0(0, 0, 0, 1);
    pink::Location l1(0, 2, 0, 3);
    pink::Location l3(0, 0, 0, 3);
    pink::Int*  i0 = new pink::Int(l1, 1);
    pink::Unop* u0 = new pink::Unop(l3, minus, i0);

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

    -) Unop::symbol == symbol;

    -) Unop::right != nullptr;

    -) Unop::ToString() == std::string(op) + i0->ToString();

    */
    result &= Test(out, "Unop::GetKind()", u0->GetKind() == pink::Ast::Kind::Unop);

    result &= Test(out, "Unop::classof()", u0->classof(u0));

    pink::Location ul(u0->GetLoc());
    result &= Test(out, "Unop::GetLoc()", ul == l3);

    result &= Test(out, "Unop::symbol", u0->op == minus);

    result &= Test(out, "Unop::term", u0-> right == i0);

    std::string unop_str = std::string(minus) + i0->ToString();

    result &= Test(out, "Unop::ToString()", u0->ToString() == unop_str);

    delete u0;

    result &= Test(out, "pink::Unop", result);
    out << "\n-----------------------\n";
    return result;
}
