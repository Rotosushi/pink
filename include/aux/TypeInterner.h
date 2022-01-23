#pragma once
#include <string>
#include <memory>

#include "type/Type.h"
#include "type/NilType.h"
#include "type/BoolType.h"
#include "type/IntType.h"


namespace pink {
    class TypeInterner {
    private:
        std::unique_ptr<NilType>  nil_type;
        std::unique_ptr<BoolType> bool_type;
        std::unique_ptr<IntType>  int_type;

    public:
        TypeInterner();
        TypeInterner(const TypeInterner& other) = delete;
        ~TypeInterner();

        NilType*  GetNilType();
        BoolType* GetBoolType();
        IntType*  GetIntType();
    };

}



// ---
