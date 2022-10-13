
#include "Test.h"
#include "aux/TestTypeInterner.h"
#include "aux/TypeInterner.h"

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
    
    pink::Type* it0 = interner.GetIntType();
    
    result &= Test(out, "TypeInterner::GetIntType(), different types compare not equal", it0 != nullptr && it0 != bt0);
    
    
    std::vector<pink::Type*> arg_tys0({bt0, bt0});
    
    pink::Type* ft0 = interner.GetFunctionType(bt0, arg_tys0);
    
    result &= Test(out, "TypeInterner::GetFunctionType()", ft0 != nullptr);
    
    pink::Type* ft1 = interner.GetFunctionType(bt0, arg_tys0);
    
    result &= Test(out, "TypeInterner::GetFunctionType(), same types compare equal", ft1 != nullptr && ft1 == ft0);
    
    std::vector<pink::Type*> arg_tys1({it0, it0});
    
    pink::Type* ft2 = interner.GetFunctionType(it0, arg_tys1);
    
    result &= Test(out, "TypeInterner::GetFunctionType(), Different Types Compare Not Equal", ft2 != nullptr && ft2 != ft1);

    result &= Test(out, "pink::TypeInterner", result);

    out << "\n-----------------------\n";
    return result;
}
