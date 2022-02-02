#pragma once
#include <string>
#include <iostream>

#include "ast/Ast.h"

namespace pink {
    // represents the literal value nil
    class Nil : public Ast {
    private:
    	virtual Outcome<Type*, Error> GetypeV(Environment& e) override;
    
    public:
        Nil(Location l);
        virtual ~Nil();

        virtual std::unique_ptr<Ast> Clone() override;

        static bool classof(const Ast* a);

        virtual std::string ToString() override;

        
        virtual Outcome<llvm::Value*, Error> Codegen(Environment& env) override;
    };
}
