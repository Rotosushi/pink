#pragma once
#include <string>

#include "type/Type.h"
#include "type/NilType.h"
#include "type/BoolType.h"
#include "type/IntType.h"


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
