
#include "aux/TypeInterner.h"

namespace pink {
    TypeInterner::TypeInterner()
        : nil_type(nullptr), bool_type(nullptr), int_type(nullptr)
    {

    }

    TypeInterner::~TypeInterner()
    {

    }

    NilType*  TypeInterner::GetNilType()
    {
        if (nil_type.get() == nullptr)
            nil_type = new NilType();

        return nil_type;
    }

    BoolType* TypeInterner::GetBoolType()
    {
        if (bool_type.get() == nullptr)
            bool_type = new BoolType();

        return bool_type;
    }

    IntType*   TypeInterner::GetIntType()
    {
        if (int_type.get() == nullptr)
            int_type = new IntType();

        return int_type;
    }
}
