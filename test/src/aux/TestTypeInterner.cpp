
#include "Test.hpp"
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
    result &= Test(out, "TypeInterner::GetNilType()", nt0 != nullptr);

    pink::Type* nt1 = interner.GetNilType();

    /*
        Checks that another of the same kind of type compares
        equal to the previous type, using pointer comparison.
    */
    result &= Test(out, "TypeInterner::GetNilType(), interned type compares equal", nt1 != nullptr && nt0 == nt1);

    pink::Type* bt0 = interner.GetBoolType();

    /*
        checks that a different type compares not equal
        to the previous type
    */
    result &= Test(out, "TypeInterner::GetBoolType(), different types compare not equal", bt0 != nullptr && nt0 != bt0);

    result &= Test(out, "pink::TypeInterner", result);

    out << "\n-----------------------\n";
    return result;
}
