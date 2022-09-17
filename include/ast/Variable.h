#pragma once
#include <string>

#include "ast/Ast.h"
#include "aux/StringInterner.h"

namespace pink {
    class Variable : public Ast {
    private:
    	virtual Outcome<Type*, Error> GetypeV(const Environment& env) override;
    	
    public:
        InternedString symbol;

        Variable(Location l, InternedString symbol);
        virtual ~Variable();

        static bool classof(const Ast* a);

        virtual std::string ToString() override;

        
        virtual Outcome<llvm::Value*, Error> Codegen(const Environment& env) override;
    };
}
