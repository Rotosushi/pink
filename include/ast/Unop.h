#pragma once
#include <string>

#include "ast/Ast.h"
#include "aux/StringInterner.h"

namespace pink {
    class Unop : public Ast {
    private:
   		virtual Outcome<Type*, Error> GetypeV(const Environment& env) override;
    public:
        InternedString op;
        std::unique_ptr<Ast> right;

    Unop(Location& loc, InternedString o, std::unique_ptr<Ast> r);
    virtual ~Unop();

    static bool classof(const Ast* t);

    virtual std::string ToString() override;
    
    virtual Outcome<llvm::Value*, Error> Codegen(const Environment& env) override;
    };
}
