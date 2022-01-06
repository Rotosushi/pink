#include "type/TestBoolType.hpp"

#include "type/BoolType.hpp"

bool TestBoolType(std::ostream& out)
{
    bool result = true;
    out << "\n-----------------------\n";
    out << "Testing and pink::BoolType: \n";

    /*
        The Type class itself only provides a small
        amount of the functionality of any given
        syntax node.

        tests:

        -) An Type node constructed with a particular kind
            holds that particular kind.

        -) classof() meets specifications:
            https://llvm.org/docs/HowToSetUpLLVMStyleRTTI.html#the-contract-of-classof

        -) An Type node constructed with a particular Location
            holds that particular Location.

        -) BoolType::ToString() == "Bool"

    */

    pink::BoolType a;

    if (a.GetKind() == pink::Type::Kind::Bool)
    {
        out << "\tTest: BoolType::GetKind(): Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest: BoolType::GetKind(): Failed\n";
    }

    if (a.classof(&a))
    {
        out << "\tTest: BoolType::classof(): Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest: BoolType::classof(): Failed\n";
    }


    std::string bool_type = "Bool";


    if (a.ToString() == bool_type)
    {
        out << "\tTest: BoolType::ToString(): Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest: BoolType::ToString(): Failed\n";
    }

    if (result)
        out << "Test: pink::BoolType: Passed\n";
    else
        out << "Test: pink::BoolType: Failed\n";
    out << "\n-----------------------\n";
    return result;
}
