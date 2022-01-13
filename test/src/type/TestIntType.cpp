#include "type/TestIntType.hpp"

#include "type/IntType.hpp"

bool TestIntType(std::ostream& out)
{
    bool result = true;
    out << "\n-----------------------\n";
    out << "Testing and pink::IntType: \n";

    /*
        The Type class itself only provides a small
        amount of the functionality of any given
        syntax node.

        tests:

        -) An Type node constructed with a particular kind
            holds that particular kind.

        -) classof() meets specifications:
            https://llvm.org/docs/HowToSetUpLLVMStyleRTTI.html#the-contract-of-classof

        -) A Type node constructed with a particular Location
            holds that particular Location.

        -) IntType::ToString() == "Nil"

    */

    pink::IntType a;

    if (a.getKind() == pink::Type::Kind::Int)
    {
        out << "\tTest: IntType::GetKind(): Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest: IntType::GetKind(): Failed\n";
    }

    if (a.classof(&a))
    {
        out << "\tTest: IntType::classof(): Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest: IntType::classof(): Failed\n";
    }

    std::string int_type = "Int";


    if (a.ToString() == int_type)
    {
        out << "\tTest: IntType::ToString(): Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest: IntType::ToString(): Failed\n";
    }

    if (result)
        out << "Test: pink::IntType: Passed\n";
    else
        out << "Test: pink::IntType: Failed\n";
    out << "\n-----------------------\n";
    return result;
}
