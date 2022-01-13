#pragma once
#include <string>

#include "type/Type.hpp"
#include "type/NilType.hpp"
#include "type/BoolType.hpp"
#include "type/IntType.hpp"


namespace pink {
    class TypeInterner {
    private:
        NilType*  nil_type;
        BoolType* bool_type;
        IntType*  int_type;

    public:
        TypeInterner();
        TypeInterner(const TypeInterner& other);
        ~TypeInterner();

        NilType*  GetNilType();
        BoolType* GetBoolType();
        IntType*  GetIntType();
    };

}



// ---
