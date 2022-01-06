#pragma once
#include <string>

namespace pink {
    class Type {
    public:
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

        Kind GetKind() const;

        virtual bool operator ==(Type& other) = 0;
        virtual std::string ToString() = 0;
    };
}
