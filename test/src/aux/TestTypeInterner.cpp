

#include "aux/TestTypeInterner.hpp"
#include "aux/TypeInterner.hpp"

bool TestTypeInterner(std::ostream& out)
{
    bool result = true;
    out << "\n-----------------------\n";
    out << "Testing Pink::TypeInterner: \n";

    pink::TypeInterner interner;

    pink::Type* nt0 = interner.GetNilType();

    /*
        Checks that the type interner returns a valid
        Interned Type
    */
    if (nt0 != NULL)
    {
        out << "\tTest: TypeInterner::GetNilType(), unique Type is valid: Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest: TypeInterner::GetNilType(), unique Type is valid: Failed\n";
    }

    pink::Type* nt1 = interner.GetNilType();

    /*
        Checks that another of the same kind of type compares
        equal to the previous type, using pointer comparison.
    */
    if (nt1 != NULL && nt0 == nt1)
    {
        out << "\tTest: TypeInterner::GetNilType(), two of the same unique Types compare equal: Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest: TypeInterner::GetNilType(), two of the same unique Types compare equal: Failed\n";
    }

    pink::Type* bt0 = interner.GetBoolType();

    /*
        checks that a different type compares not equal
        to the previous type
    */
    if (bt0 != NULL && nt0 != bt0)
    {
        out << "\tTest: TypeInterner::GetNilType(), two different unique Types compares not-equal: Passed\n";
    }
    else
    {
        result = false;
        out << "\tTest: TypeInterner::GetNilType(), two different unique Types compares not-equal: Failed\n";
    }

    if (result)
        out << "Test: pink::TypeInterner: Passed\n";
    else
        out << "Test: pink::TypeInterner: Failed\n";

    out << "\n-----------------------\n";
    return result;
}
