#pragma once
#include <string>

#include "type/Type.h"

namespace pink {
    class IntType : public Type {
    public:
        IntType();
        virtual ~IntType();

        static bool classof(const Type* t);

        virtual bool operator==(Type& other) override;
        virtual std::string ToString() override;
    };
}
