
#include "ast/TestBind.hpp"

#include "ast/Nil.hpp"
#include "ast/Bind.hpp"

bool TestBind(std::ostream& out)
{
    bool result = true;
    result = true;

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
    if (b0->GetKind() == pink::Ast::Kind::Bind)
    {
        out << "\tTest Bind::GetKind(): Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest Bind::GetKind(): Failed\n";
    }

    if (b0->classof(b0))
    {
        out << "\tTest: Bind::classof(): Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest: Bind::classof(): Failed\n";
    }

    pink::Location l2(l0);
    pink::Location bl(b0->GetLoc());
    if (bl.firstLine == l2.firstLine && bl.firstColumn == l2.firstColumn
     && bl.lastLine == l2.lastLine && bl.lastColumn == l2.lastColumn)
    {
        out << "\tTest: Bind::GetLoc(): Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest: Bind::GetLoc(): Failed\n";
    }

    if (b0->symbol == v)
    {
        out << "\tTest: Bind::symbol: Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest: Bind::symbol: Failed\n";
    }

    if (b0->term == n0)
    {
        out << "\tTest: Bind::term: Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest: Bind::term: Failed\n";
    }

    std::string bind_str = std::string(v) + std::string(" := ") + n0->ToString();

    if (b0->ToString() == bind_str)
    {
        out << "\tTest: Bind::ToString: Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest: Bind::ToString: Failed\n";
    }

    delete b0;

    if (result)
        out << "Test: pink::Bind: Passed\n";
    else
        out << "Test: pink::Bind: Failed\n";
    out << "\n-----------------------\n";
    return result;
}
