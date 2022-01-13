#include <string>
#include <sstream>

#include "Test.hpp"
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

    result &= Test(out, "Nil::GetKind()", a.getKind() == pink::Ast::Kind::Nil);
    result &= Test(out, "Nil::classof()", a.classof(&a));

    pink::Location l1(l0);

    result &= Test(out, "Nil::GetLoc()", l0 == l1);

    std::string nil = "nil";

    result &= Test(out, "Nil::ToString()", a.ToString() == nil);

    result &= Test(out, "pink::Nil", result);
    out << "\n-----------------------\n";
    return result;
}
