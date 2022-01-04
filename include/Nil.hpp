#pragma once
#include <string>
#include <iostream>

#include "Ast.hpp"

namespace pink {
    // represents the literal value nil
    class Nil : public Ast {
    public:
        Nil(Location l);
        virtual ~Nil();

        virtual std::string ToString();
        virtual std::ostream& Print(std::ostream& out));

        // virtual std::variant<pink::Error, pink::Type*> Getype(Environment& e);
        // virtual std::variant<pink::Error, pink::Type*> ToLLVM(Environment& e);
    };
}
