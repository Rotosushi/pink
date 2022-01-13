#include "Test.hpp"
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
    result &= Test(out, "Binop::GetKind()", b0->getKind() == pink::Ast::Kind::Binop);

    result &= Test(out, "Binop::classof()", b0->classof(b0));

    pink::Location bl(b0->GetLoc());

    result &= Test(out, "Binop::GetLoc()", bl == l3);

    result &= Test(out, "Binop::symbol", b0->op == plus);

    result &= Test(out, "Binop::term", b0->left == i0 && b0->right == i1);


    std::string binop_str = i0->ToString() + " " + std::string(plus) + " " + i1->ToString();

    result &= Test(out, "Binop::ToString()", b0->ToString() == binop_str);

    delete b0;

    result &= Test(out, "pink::Binop", result);
    out << "\n-----------------------\n";
    return result;
}
