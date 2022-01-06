#pragma once
#include <string>

#include "type/Type.hpp"

namespace pink {
    class BoolType : public Type {
    public:
        BoolType();
        virtual ~BoolType();

        static bool classof(const Type* t);

        virtual bool operator==(Type& other) override;
        virtual std::string ToString() override;
    };
}
