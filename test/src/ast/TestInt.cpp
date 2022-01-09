#include "Test.hpp"
#include "ast/TestInt.hpp"

#include "ast/Int.hpp"


bool TestInt(std::ostream& out)
{
    int result = true;
    result = true;

    out << "\n-----------------------\n";
    out << "Testing pink::Int: \n";

    pink::Location l0(0, 3, 0, 7);
    pink::Int* i0 = new pink::Int(l0, 42);

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

        -) Int::ToString() == "42"
    */
    result &= Test(out, "Int::GetKind()", i0->GetKind() == pink::Ast::Kind::Int);

    result &= Test(out, "Int::classof()", i0->classof(i0));

    pink::Location l1(l0);
    pink::Location il(i0->GetLoc());
    result &= Test(out, "Int::GetLoc()", il == l1);

    result &= Test(out, "Int::value", i0->value == 42);

    result &= Test(out, "Int::ToString()", i0->ToString() == "42");

    delete i0;

    result &= Test(out, "pink::Int", result);
    out << "\n-----------------------\n";
    return result;
}
