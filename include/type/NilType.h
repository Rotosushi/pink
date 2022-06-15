#pragma once
#include <string>

#include "type/Type.h"

namespace pink {
    class NilType : public Type {
    public:
        NilType();
        virtual ~NilType();

        static bool classof(const Type* t);

        virtual bool EqualTo(Type* other) override;
        virtual std::string ToString() override;
        
        virtual Outcome<llvm::Type*, Error> Codegen(std::shared_ptr<Environment> env) override;
    };
}
