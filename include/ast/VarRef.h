#pragma once
#include <string>

#include "ast/Ast.h"
#include "aux/StringInterner.h"

namespace pink {
    class VarRef : public Ast {
    private:
    	virtual Outcome<Type*, Error> GetypeV(std::shared_ptr<Environment> env) override;
    	
    public:
        InternedString symbol;

        VarRef(Location l, InternedString symbol);
        virtual ~VarRef();

        virtual std::unique_ptr<Ast> Clone() override;

        static bool classof(const Ast* a);

        virtual std::string ToString() override;

        virtual Outcome<llvm::Value*, Error> Codegen(std::shared_ptr<Environment> env) override;
    };
}
