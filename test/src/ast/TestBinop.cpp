
#include "ast/TestBinop.hpp"

#include "ast/Binop.hpp"
#include "ast/Int.hpp"

bool TestBinop(std::ostream& out)
{
    bool result = true;
    result = true;

    out << "\n-----------------------\n";
    out << "Testing pink::Binop: \n";

    pink::StringInterner s;
    pink::InternedString plus = s.Intern("+");
    pink::Location l0(0, 0, 0, 1);
    pink::Location l1(0, 2, 0, 3);
    pink::Location l2(0, 4, 0, 5);
    pink::Location l3(0, 0, 0, 5);
    pink::Int*   i0 = new pink::Int(l0, 1);
    pink::Int*   i1 = new pink::Int(l2, 1);
    pink::Binop* b0 = new pink::Binop(l3, plus, i0, i1);

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

    -) Binop::symbol == symbol;

    -) Binop::term != nullptr;

    -) Binop::ToString() == l->ToString() + std::string(op) + r->ToString();

    */
    if (b0->GetKind() == pink::Ast::Kind::Binop)
    {
        out << "\tTest Binop::GetKind(): Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest Binop::GetKind(): Failed\n";
    }

    if (b0->classof(b0))
    {
        out << "\tTest: Binop::classof(): Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest: Binop::classof(): Failed\n";
    }

    pink::Location bl(b0->GetLoc());
    if (bl.firstLine == l3.firstLine && bl.firstColumn == l3.firstColumn
     && bl.lastLine == l3.lastLine && bl.lastColumn == l3.lastColumn)
    {
        out << "\tTest: Binop::GetLoc(): Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest: Binop::GetLoc(): Failed\n";
    }

    if (b0->op == plus)
    {
        out << "\tTest: Binop::symbol: Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest: Binop::symbol: Failed\n";
    }

    if (b0->left == i0 && b0->right == i1)
    {
        out << "\tTest: Binop::term: Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest: Binop::term: Failed\n";
    }

    std::string binop_str = i0->ToString() + " " + std::string(plus) + " " + i1->ToString();

    if (b0->ToString() == binop_str)
    {
        out << "\tTest: Binop::ToString: Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest: Binop::ToString: Failed\n";
    }

    delete b0;

    if (result)
        out << "Test: pink::Binop: Passed\n";
    else
        out << "Test: pink::Binop: Failed\n";
    out << "\n-----------------------\n";
    return result;
}
