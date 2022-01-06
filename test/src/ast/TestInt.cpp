
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
    if (i0->GetKind() == pink::Ast::Kind::Int)
    {
        out << "\tTest Int::GetKind(): Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest Int::GetKind(): Failed\n";
    }

    if (i0->classof(i0))
    {
        out << "\tTest: Int::classof(): Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest: Int::classof(): Failed\n";
    }

    pink::Location l1(l0);
    pink::Location il(i0->GetLoc());
    if (il.firstLine == l1.firstLine && il.firstColumn == l1.firstColumn
     && il.lastLine == l1.lastLine && il.lastColumn == l1.lastColumn)
    {
        out << "\tTest: Int::GetLoc(): Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest: Int::GetLoc(): Failed\n";
    }


    if (i0->value == 42)
    {
        out << "\tTest Int::value: Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest Int::value: Failed\n";
    }

    if (i0->ToString() == "42")
    {
        out << "\tTest: Int::ToString(): Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest: Int::ToString(): Failed\n";
    }

    delete i0;

    if (result)
        out << "Test: pink::Int: Passed\n";
    else
        out << "Test: pink::Int: Failed\n";
    out << "\n-----------------------\n";
    return result;
}
