#pragma once
#include <string>

#include "aux/Outcome.h"

#include "llvm/IR/Type.h"

namespace pink {
	class Environment;
    class Type {
    public:
        // https://llvm.org/docs/HowToSetUpLLVMStyleRTTI.html
        enum class Kind {
            Nil,
            Bool,
            Int,
            
            Function,
            Pointer,
            Array,
        };

    protected:
        Kind     kind;

    public:
        Type(Kind k);
        virtual ~Type();

        Kind getKind() const;

        /*
            since types are interned, we can use pointer
            comparison as a replacement for simple equality.
            However, types still need to sometimes be compared
            structurally, for instance within the type interner
            itself. This is accomplished with EqualTo
        */
        virtual bool EqualTo(Type* other) = 0;
        virtual std::string ToString() = 0;

        virtual Outcome<llvm::Type*, Error> Codegen(std::shared_ptr<Environment> env) = 0;
    };
}
