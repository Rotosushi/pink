#pragma once
#include <vector>
#include <string>
#include <memory>

#include "type/Type.h"
#include "type/NilType.h"
#include "type/BoolType.h"
#include "type/IntType.h"
#include "type/VoidType.h"
#include "type/FunctionType.h"
#include "type/PointerType.h"
#include "type/ArrayType.h"
#include "type/TupleType.h"

namespace pink {
    class TypeInterner {
    private:
        std::unique_ptr<NilType>  nil_type;
        std::unique_ptr<BoolType> bool_type;
        std::unique_ptr<IntType>  int_type;
        std::unique_ptr<VoidType> void_type;
        std::vector<std::unique_ptr<FunctionType>> function_types;
        std::vector<std::unique_ptr<PointerType>> pointer_types;
        std::vector<std::unique_ptr<ArrayType>> array_types;
        std::vector<std::unique_ptr<TupleType>> tuple_types;

    public:
        TypeInterner();
        TypeInterner(const TypeInterner& other) = delete;
        ~TypeInterner();

        NilType*  GetNilType();
        BoolType* GetBoolType();
        IntType*  GetIntType();
        VoidType* GetVoidType();
        FunctionType* GetFunctionType(Type* ret_type, std::vector<Type*> arg_types);
        PointerType*  GetPointerType(Type* pointee_type);
        ArrayType* GetArrayType(size_t size, Type* member_type);
        TupleType* GetTupleType(std::vector<Type*> member_types);
    };

}



// ---
