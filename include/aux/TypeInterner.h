/**
 * @file TypeInterner.h
 * @brief Header for class TypeInterner
 * @version 0.1
 * 
 */
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
    /**
     * @brief Interns Types, so that only one instance of each unique type is constructed
     * 
     * 
     */
    class TypeInterner {
    private:
        /**
         * @brief The one instance of a NilType
         * 
         */
        std::unique_ptr<NilType>  nil_type;

        /**
         * @brief The one instance of a BoolType
         * 
         */
        std::unique_ptr<BoolType> bool_type;

        /**
         * @brief The one instance of an IntType
         * 
         */
        std::unique_ptr<IntType>  int_type;

        /**
         * @brief The one instance of a VoidType
         * 
         */
        std::unique_ptr<VoidType> void_type;

        /**
         * @brief A vector of all the unique FunctionTypes
         * 
         */
        std::vector<std::unique_ptr<FunctionType>> function_types;

        /**
         * @brief A vector of all the unique PointerTypes
         * 
         */
        std::vector<std::unique_ptr<PointerType>> pointer_types;

        /**
         * @brief A vector of all the unique ArrayTypes
         * 
         */
        std::vector<std::unique_ptr<ArrayType>> array_types;

        /**
         * @brief A vector of all the unique TupleTypes
         * 
         */
        std::vector<std::unique_ptr<TupleType>> tuple_types;

    public:
        /**
         * @brief Construct a new Type Interner
         * 
         */
        TypeInterner();

        /**
         * @brief We disallow a TypeInterner to be constructed from an existing TypeInterner
         * 
         * @param other the other Type Interner
         */
        TypeInterner(const TypeInterner& other) = delete;

        /**
         * @brief Destroy the Type Interner
         * 
         */
        ~TypeInterner();

        /**
         * @brief Get the instance of a NilType 
         * 
         * @return NilType* the NilType
         */
        NilType*  GetNilType();

        /**
         * @brief Get the instance of a BoolType
         * 
         * @return BoolType* the BoolType
         */
        BoolType* GetBoolType();

        /**
         * @brief Get the instance of an IntType
         * 
         * @return IntType* the IntType
         */
        IntType*  GetIntType();

        /**
         * @brief Get the instance of a VoidType
         * 
         * @return VoidType* the VoidType
         */
        VoidType* GetVoidType();

        /**
         * @brief Get the instance of a FunctionType
         * 
         * @param ret_type The return type of the FunctionType
         * @param arg_types the argument types of the FunctionType
         * @return FunctionType* the FunctionType
         */
        FunctionType* GetFunctionType(Type* ret_type, std::vector<Type*> arg_types);

        /**
         * @brief Get the instance of a PointerType
         * 
         * @param pointee_type the pointee type of the PointerType
         * @return PointerType* the PointerType
         */
        PointerType*  GetPointerType(Type* pointee_type);

        /**
         * @brief Get the instance of a ArrayType
         * 
         * @param size the size of the ArrayType
         * @param member_type the member type of the ArrayType
         * @return ArrayType* the ArrayType
         */
        ArrayType* GetArrayType(size_t size, Type* member_type);

        /**
         * @brief Get the instance of a TupleType
         * 
         * @param member_types the member types of the TupleType
         * @return TupleType* the TupleType
         */
        TupleType* GetTupleType(std::vector<Type*> member_types);
    };

}



// ---
