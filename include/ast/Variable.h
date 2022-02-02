#pragma once
#include <string>

#include "ast/Ast.h"
#include "aux/StringInterner.h"

namespace pink {
    class Variable : public Ast {
    private:
    	virtual Outcome<Type*, Error> GetypeV(Environment& env) override;
    	
    public:
        InternedString symbol;

        Variable(Location l, InternedString symbol);
        virtual ~Variable();

        virtual std::unique_ptr<Ast> Clone() override;

        static bool classof(const Ast* a);

        virtual std::string ToString() override;

        
        virtual Outcome<llvm::Value*, Error> Codegen(Environment& env) override;
    };
}
