#pragma once
#include <memory>
#include <string>
#include <iostream>

#include "llvm/Support/Casting.h"

#include "aux/Location.h"
#include "aux/Error.h"
#include "aux/Outcome.h"

#include "type/Type.h"

#include "llvm/IR/Value.h"

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

        virtual std::unique_ptr<Ast> Clone() = 0;

		/*
		#TODO
			This function ends up doing a lot of 
			intermediate allocations and concatenations,
			perhaps we could rewrite this to use 
			llvm::Twine or something similar?
		*/
        virtual std::string ToString() = 0;

        /*
        	#TODO: Getype can be made to cache 
        		the computed type in the Ast 
        		class itself, which should save
        		on computation within Codegen.
        */
        virtual Outcome<Type*, Error> Getype(Environment& e) = 0;
        virtual Outcome<llvm::Value*, Error> Codegen(Environment& env) = 0;
    };
}
