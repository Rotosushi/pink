
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
    if (u0->GetKind() == pink::Ast::Kind::Unop)
    {
        out << "\tTest Unop::GetKind(): Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest Unop::GetKind(): Failed\n";
    }

    if (u0->classof(u0))
    {
        out << "\tTest: Unop::classof(): Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest: Unop::classof(): Failed\n";
    }

    pink::Location ul(u0->GetLoc());
    if (ul.firstLine == l3.firstLine && ul.firstColumn == l3.firstColumn
     && ul.lastLine == l3.lastLine && ul.lastColumn == l3.lastColumn)
    {
        out << "\tTest: Unop::GetLoc(): Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest: Unop::GetLoc(): Failed\n";
    }

    if (u0->op == minus)
    {
        out << "\tTest: Unop::symbol: Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest: Unop::symbol: Failed\n";
    }

    if (u0->right == i0)
    {
        out << "\tTest: Unop::term: Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest: Unop::term: Failed\n";
    }

    std::string unop_str = std::string(minus) + i0->ToString();

    if (u0->ToString() == unop_str)
    {
        out << "\tTest: Unop::ToString: Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest: Unop::ToString: Failed\n";
    }

    delete u0;

    if (result)
        out << "Test: pink::Unop: Passed\n";
    else
        out << "Test: pink::Unop: Failed\n";
    out << "\n-----------------------\n";
    return result;
}
