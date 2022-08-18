#pragma once
#include <vector>
#include <string>
#include <memory>

#include "type/Type.h"
#include "type/NilType.h"
#include "type/BoolType.h"
#include "type/IntType.h"
#include "type/FunctionType.h"
#include "type/PointerType.h"
#include "type/ArrayType.h"

namespace pink {
    class TypeInterner {
    private:
        std::unique_ptr<NilType>  nil_type;
        std::unique_ptr<BoolType> bool_type;
        std::unique_ptr<IntType>  int_type;
        std::vector<std::unique_ptr<FunctionType>> function_types;
        std::vector<std::unique_ptr<PointerType>> pointer_types;
        std::vector<std::unique_ptr<ArrayType>> array_types;

    public:
        TypeInterner();
        TypeInterner(const TypeInterner& other) = delete;
        ~TypeInterner();

        NilType*  GetNilType();
        BoolType* GetBoolType();
        IntType*  GetIntType();
        FunctionType* GetFunctionType(Type* r, std::vector<Type*> a);
        PointerType*  GetPointerType(Type* pointee_ty);
        ArrayType* GetArrayType(size_t size, Type* member_ty);
    };

}



// ---
