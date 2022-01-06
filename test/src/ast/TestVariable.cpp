
#include "ast/TestVariable.hpp"

#include "ast/Variable.hpp"

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
    if (v0->GetKind() == pink::Ast::Kind::Variable)
    {
        out << "\tTest Variable::GetKind(): Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest Variable::GetKind(): Failed\n";
    }

    if (v0->classof(v0))
    {
        out << "\tTest: Variable::classof(): Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest: Variable::classof(): Failed\n";
    }

    pink::Location l1(l0);
    pink::Location vl(v0->GetLoc());
    if (vl.firstLine == l1.firstLine && vl.firstColumn == l1.firstColumn
     && vl.lastLine == l1.lastLine && vl.lastColumn == l1.lastColumn)
    {
        out << "\tTest: Variable::GetLoc(): Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest: Variable::GetLoc(): Failed\n";
    }


    if (v0->symbol == v)
    {
        out << "\tTest Variable::symbol: Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest Variable::symbol: Failed\n";
    }

    if (v0->ToString() == v)
    {
        out << "\tTest: Variable::ToString(): Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest: Variable::ToString(): Failed\n";
    }

    delete v0;

    if (result)
        out << "Test: pink::Variable: Passed\n";
    else
        out << "Test: pink::Variable: Failed\n";
    out << "\n-----------------------\n";
    return result;
}
