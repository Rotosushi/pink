#include "Test.h"
#include "ast/TestVariable.h"

#include "ast/Variable.h"

bool TestVariable(std::ostream& out)
{
    bool result = true;
    result = true;

    out << "\n-----------------------\n";
    out << "Testing pink::Variable: \n";

    pink::StringInterner s;
    pink::InternedString v = s.Intern("v");
    pink::Location l0(0, 3, 0, 7);
    pink::Variable* v0 = new pink::Variable(l0, v);

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

        -) Variable::ToString() == "v"
    */
    result &= Test(out, "Variable::GetKind()", v0->getKind() == pink::Ast::Kind::Variable);

    result &= Test(out, "Variable::classof()", v0->classof(v0));

    pink::Location l1(l0);
    pink::Location vl(v0->GetLoc());
    result &= Test(out, "Variable::GetLoc()", vl == l1);

    result &= Test(out, "Variable::symbol", v0->symbol == v);

    result &= Test(out, "Variable::ToString()", v0->ToString() == v);

    delete v0;

    result &= Test(out, "pink::Variable", result);
    out << "\n-----------------------\n";
    return result;
}
