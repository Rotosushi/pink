
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
            nil_type = std::make_unique<NilType>();

        return nil_type.get();
    }

    BoolType* TypeInterner::GetBoolType()
    {
        if (bool_type.get() == nullptr)
            bool_type = std::make_unique<BoolType>();

        return bool_type.get();
    }

    IntType*   TypeInterner::GetIntType()
    {
        if (int_type.get() == nullptr)
            int_type = std::make_unique<IntType>();

        return int_type.get();
    }
}
