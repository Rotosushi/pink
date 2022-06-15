#pragma once
#include <string>

#include "ast/Ast.h"

namespace pink {
    class Int : public Ast {
    private:
    	virtual Outcome<Type*, Error> GetypeV(std::shared_ptr<Environment> e) override;
    
    public:
        int value;

        Int(Location l, int i);
        virtual ~Int();

        virtual std::unique_ptr<Ast> Clone() override;

        static bool classof(const Ast* a);

        virtual std::string ToString() override;

        
        virtual Outcome<llvm::Value*, Error> Codegen(std::shared_ptr<Environment> env) override;
    };
}
