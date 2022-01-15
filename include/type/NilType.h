#pragma once
#include <string>

#include "type/Type.h"

namespace pink {
    class NilType : public Type {
    public:
        NilType();
        virtual ~NilType();

        static bool classof(const Type* t);

        virtual bool operator==(Type& other) override;
        virtual std::string ToString() override;
    };
}
