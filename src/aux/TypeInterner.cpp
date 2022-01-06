
#include "aux/TypeInterner.hpp"

namespace pink {
    TypeInterner::TypeInterner()
        : nil_type(nullptr), bool_type(nullptr), int_type(nullptr)
    {

    }

    TypeInterner::TypeInterner(const TypeInterner& other)
        : nil_type(other.nil_type), bool_type(other.bool_type),
          int_type(other.int_type)
    {

    }

    TypeInterner::~TypeInterner()
    {
        delete nil_type;
        delete bool_type;
        delete int_type;
    }

    NilType*  TypeInterner::GetNilType()
    {
        if (nil_type == nullptr)
            nil_type = new NilType();

        return nil_type;
    }

    BoolType* TypeInterner::GetBoolType()
    {
        if (bool_type == nullptr)
            bool_type = new BoolType();

        return bool_type;
    }

    IntType*   TypeInterner::GetIntType()
    {
        if (int_type == nullptr)
            int_type = new IntType();

        return int_type;
    }
}
