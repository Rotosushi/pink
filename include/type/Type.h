#pragma once
#include <string>

namespace pink {
    class Type {
    public:
        // https://llvm.org/docs/HowToSetUpLLVMStyleRTTI.html
        enum class Kind {
            Nil,
            Bool,
            Int,
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
            itself. This is accomplished with the overloaded operator==
        */
        virtual bool operator ==(Type& other) = 0;
        virtual std::string ToString() = 0;
    };
}