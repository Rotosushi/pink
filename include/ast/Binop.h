#pragma once
#include <string>

#include "ast/Ast.h"
#include "aux/StringInterner.h"

namespace pink {
    class Binop : public Ast {
    public:
        InternedString       op;
        std::unique_ptr<Ast> left;
        std::unique_ptr<Ast> right;

    Binop(Location& loc, InternedString o, std::unique_ptr<Ast> l, std::unique_ptr<Ast> r);
    virtual ~Binop();

    virtual std::unique_ptr<Ast> Clone() override;

    static bool classof(const Ast* t);

    virtual std::string ToString() override;

    virtual Outcome<Type*, Error> Getype(Environment& env) override;
    virtual Outcome<llvm::Value*, Error> Codegen(Environment& env) override;
    };
}
