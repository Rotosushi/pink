#include "type/TestTypeAndNilType.hpp"

#include "type/NilType.hpp"


bool TestTypeAndNilType(std::ostream& out)
{
    bool result = true;
    out << "\n-----------------------\n";
    out << "Testing pink::Type and pink::NilType: \n";

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

        -) NilType::ToString() == "Nil"

    */


    pink::NilType a;

    if (a.GetKind() == pink::Type::Kind::Nil)
    {
        out << "\tTest: NilType::GetKind(): Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest: NilType::GetKind(): Failed\n";
    }

    if (a.classof(&a))
    {
        out << "\tTest: NilType::classof(): Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest: NilType::classof(): Failed\n";
    }


    std::string nil_type = "Nil";


    if (a.ToString() == nil_type)
    {
        out << "\tTest: NilType::ToString(): Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest: NilType::ToString(): Failed\n";
    }

    if (result)
        out << "Test: pink::Type and pink::NilType: Passed\n";
    else
        out << "Test: pink::Type and pink::NilType: Failed\n";
    out << "\n-----------------------\n";
    return result;
}
