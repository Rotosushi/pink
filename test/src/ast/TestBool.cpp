
#include "ast/TestBool.hpp"

#include "ast/Bool.hpp"

bool TestBool(std::ostream& out)
{
    bool result = true;
    result = true;

    out << "\n-----------------------\n";
    out << "Testing pink::Bool: \n";

    pink::Location l0(0, 3, 0, 9);
    pink::Bool* b0 = new pink::Bool(l0, true);

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

        -) Bool::ToString() == "Bool"
    */
    if (b0->GetKind() == pink::Ast::Kind::Bool)
    {
        out << "\tTest Bool::GetKind(): Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest Bool::GetKind(): Failed\n";
    }

    if (b0->classof(b0))
    {
        out << "\tTest: Bool::classof(): Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest: Bool::classof(): Failed\n";
    }

    pink::Location l1(l0);
    pink::Location bl(b0->GetLoc());
    if (bl.firstLine == l1.firstLine && bl.firstColumn == l1.firstColumn
     && bl.lastLine == l1.lastLine && bl.lastColumn == l1.lastColumn)
    {
        out << "\tTest: Bool::GetLoc(): Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest: Bool::GetLoc(): Failed\n";
    }

    if (b0->value == true)
    {
        out << "\tTest Bool::value: Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest Bool::value: Failed\n";
    }

    if (b0->ToString() == "true")
    {
        out << "\tTest Bool::ToString: Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest Bool::ToString: Failed\n";
    }

    delete b0;

    if (result)
        out << "Test: pink::Bool: Passed\n";
    else
        out << "Test: pink::Bool: Failed\n";
    out << "\n-----------------------\n";
    return result;
}
