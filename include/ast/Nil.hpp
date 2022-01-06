#pragma once
#include <string>
#include <iostream>

#include "ast/Ast.hpp"

namespace pink {
    // represents the literal value nil
    class Nil : public Ast {
    public:
        Nil(Location l);
        virtual ~Nil();

        virtual Ast* Clone() override;

        static bool classof(const Ast* a);

        virtual std::string ToString() override;

        // virtual std::variant<pink::Error, pink::Type*> Getype(Environment& e);
        // virtual std::variant<pink::Error, llvm::Value*> ToLLVM(Environment& e);
    };
}
