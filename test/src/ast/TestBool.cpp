#include "Test.h"
#include "ast/TestBool.h"

#include "ast/Bool.h"

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
    result &= Test(out, "Bool::GetKind()", b0->getKind() == pink::Ast::Kind::Bool);

    result &= Test(out, "Bool::classof()", b0->classof(b0));

    pink::Location l1(l0);
    pink::Location bl(b0->GetLoc());
    result &= Test(out, "Bool::GetLoc()", bl == l1);

    result &= Test(out, "Bool::value", b0->value == true);

    result &= Test(out, "Bool::ToString()", b0->ToString() == "true");

    delete b0;

    result &= Test(out, "pink::Result", result);
    out << "\n-----------------------\n";
    return result;
}
