#pragma once

#include <string>
#include <iostream>

#include "llvm/Support/Casting.h"

#include "aux/Location.h"
#include "aux/Error.h"
#include "aux/Outcome.h"

#include "type/Type.h"

namespace pink {
    class Environment;

    class Ast {
    public:
        // https://llvm.org/docs/HowToSetUpLLVMStyleRTTI.html
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

        Kind getKind() const;
        Location GetLoc() const;

        virtual Ast* Clone() = 0;

		/*
			This function ends up doing a lot of 
			intermediate allocations and concatenations,
			perhaps we could rewrite this to use 
			llvm::Twine or something similar?
		*/
        virtual std::string ToString() = 0;

        
        virtual Outcome<Type*, Error> Getype(Environment& e) = 0;
        //virtual std::variant<pink::Error, llvm::Value*> ToLLVM(Environment& e) = 0;
    };
}
