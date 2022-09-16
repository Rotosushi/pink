#pragma once
#include <string>

#include "ast/Ast.h"

namespace pink {
    class Bool : public Ast {
    private:
    	virtual Outcome<Type*, Error> GetypeV(const Environment& e) override;
    
    public:
        bool value;

        Bool(Location l, bool b);
        virtual ~Bool();

        virtual std::unique_ptr<Ast> Clone() override;

        static bool classof(const Ast* a);

        virtual std::string ToString() override;

        
        virtual Outcome<llvm::Value*, Error> Codegen(const Environment& env) override;
    };
}
