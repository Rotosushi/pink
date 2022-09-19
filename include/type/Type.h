/**
 * @file Type.h
 * @brief Header for class Type
 * @version 0.1
 */
#pragma once
#include <string>

#include "aux/Outcome.h"

#include "llvm/IR/Type.h"

namespace pink {
	class Environment;

    /**
     * @brief Represents an instance of a Type
     * 
     * \note Type is pure virtual, so there is no way to construct a plain Type,
     * only an instance of a derived Type may be constructed
     */
    class Type {
    public:
        /**
         * @brief Type::Kind is deifined so as to conform to LLVM style [RTTI]
         * 
         * [RTTI]: https://llvm.org/docs/HowToSetUpLLVMStyleRTTI.html "RTTI"
         * 
         * Type::Kind is used as a tag to identify which kind of type is currently 
         * constructed that a given Type* points to.
         *
         */
        enum class Kind {
            Void,
            Nil,
            Bool,
            Int,
            
            Function,
            Pointer,
            Array,
            Tuple,
        };

    protected:
        /**
         * @brief The kind of this particular Type
         * 
         */
        Kind     kind;

    public:
        /**
         * @brief Construct a new Type
         * 
         * @param k the kind of Type being constructed
         */
        Type(Kind k);

        /**
         * @brief Destroy the Type
         * 
         */
        virtual ~Type();

        /**
         * @brief Get the Kind of this Type
         * 
         * @return Kind the Type::Kind of this Type
         */
        Kind getKind() const;

        /*
            
        */

        /**
         * @brief Computes Equality of this type and other type.
         * 
         * \note since types are interned, we can use pointer
         *  comparison as a replacement for simple equality.
         *  However, types still need to sometimes be compared
         *  structurally, for instance within the type interner
         *  itself. This is what is accomplished with EqualTo
         * 
         * @param other the other type to compare against
         * @return true if other *is* equivalent to this type
         * @return false if other *is not* equivalent to this type
         */
        virtual bool EqualTo(Type* other) = 0;

        /**
         * @brief Computes the cannonical string representation of this Type
         * 
         * @return std::string the string representation
         */
        virtual std::string ToString() = 0;

        /**
         * @brief Computes the llvm::Type equivalent to this Type
         * 
         * @param env the environment of this compilation unit
         * @return Outcome<llvm::Type*, Error> if true then the llvm::Type equivalent to this Type,
         * if false then the Error encountered
         */
        virtual Outcome<llvm::Type*, Error> Codegen(const Environment& env) = 0;
    };
}
