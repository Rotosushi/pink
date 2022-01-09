#pragma once

#include <string>
#include <iostream>

#include "llvm/Support/Casting.h"

#include "aux/Location.hpp"
#include "aux/Error.hpp"


namespace pink {
    class Ast {
    public:
        // at first, what we are targeting is a c-evaluation calculator
        // then we will add functions. This way we can more clearly
        // test the functionality of just the functions.
        enum class Kind {
            Variable,
            Bind,
            Assignment,

            Binop,
            Unop,

            Nil,
            Bool,
            Int,
        };

    protected:
        const Kind kind;
        Location   loc;

    public:
        Ast(const Kind k, Location l);
        virtual ~Ast();

        Kind GetKind() const;
        Location GetLoc() const;

        virtual Ast* Clone() = 0;

        virtual std::string ToString() = 0;

        // virtual std::variant<pink::Error, pink::Type*> Getype(Environment& e) = 0;
        // virtual std::variant<pink::Error, llvm::Value*> ToLLVM(Environment& e) = 0;
    };
}