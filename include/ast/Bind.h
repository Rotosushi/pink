#pragma once

#include "aux/StringInterner.h"
#include "ast/Ast.h"

namespace pink {
    class Bind : public Ast {
    private:
    	virtual Outcome<Type*, Error> GetypeV(Environment& e) override;
    
    public:
        InternedString symbol;
        std::unique_ptr<Ast> term;

        Bind(Location l, InternedString i, std::unique_ptr<Ast> t);
        virtual ~Bind();

        virtual std::unique_ptr<Ast> Clone() override;

        static bool classof(const Ast* a);

        virtual std::string ToString() override;

        
        virtual Outcome<llvm::Value*, Error> Codegen(Environment& env) override;
    };
}
