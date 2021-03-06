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
            VarRef,
            Bind,
            Assignment,
            Block,
            Function,

            Binop,
            Unop,

            Nil,
            Bool,
            Int,
        };

    protected:
        const Kind kind;
        Location   loc;
        Type*      type;
        
    private:
    	virtual Outcome<Type*, Error> GetypeV(Environment& env) = 0;

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
			
		#TODO
			This function should be aware of the level of 
			nesting, and properly indent the code it emits.
		*/
        virtual std::string ToString() = 0;

        /*
        	#TODO: Getype can be made to cache 
        		the computed type in the Ast 
        		class itself, which should save
        		on computation within Codegen.
        */
        Outcome<Type*, Error> Getype(Environment& e);
        virtual Outcome<llvm::Value*, Error> Codegen(Environment& env) = 0;
    };
}
