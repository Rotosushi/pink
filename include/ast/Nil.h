#pragma once
#include <string>
#include <iostream>

#include "ast/Ast.h"

namespace pink {
    // represents the literal value nil
    class Nil : public Ast {
    public:
        Nil(Location l);
        virtual ~Nil();

        virtual std::unique_ptr<Ast> Clone() override;

        static bool classof(const Ast* a);

        virtual std::string ToString() override;

        virtual Outcome<Type*, Error> Getype(Environment& e) override;
        virtual Outcome<llvm::Value*, Error> Codegen(Environment& env) override;
    };
}
