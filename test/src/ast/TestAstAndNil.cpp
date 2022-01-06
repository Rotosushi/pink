#include <string>
#include <sstream>

#include "ast/TestAstAndNil.hpp"
#include "ast/Nil.hpp"


bool TestAstAndNil(std::ostream& out)
{
    bool result = true;
    out << "\n-----------------------\n";
    out << "Testing pink::Ast and pink::Nil: \n";

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

        -) Nil::ToString() == "nil"

    */


    pink::Location l0(0, 14, 0, 15);
    pink::Nil a(l0);

    if (a.GetKind() == pink::Ast::Kind::Nil)
    {
        out << "\tTest: Nil::GetKind(): Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest: Nil::GetKind(): Failed\n";
    }

    if (a.classof(&a))
    {
        out << "\tTest: Nil::classof(): Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest: Nil::classof(): Failed\n";
    }

    pink::Location l1(l0);
    if (l0.firstLine == l1.firstLine && l0.firstColumn == l1.firstColumn
     && l0.lastLine == l1.lastLine && l0.lastColumn == l1.lastColumn)
    {
        out << "\tTest: Nil::GetLoc(): Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest: Nil::GetLoc(): Failed\n";
    }


    std::string nil = "nil";


    if (a.ToString() == nil)
    {
        out << "\tTest: Nil::ToString(): Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest: Nil::ToString(): Failed\n";
    }

    if (result)
        out << "Test: pink::Ast and pink::Nil: Passed\n";
    else
        out << "Test: pink::Ast and pink::Nil: Failed\n";
    out << "\n-----------------------\n";
    return result;
}
