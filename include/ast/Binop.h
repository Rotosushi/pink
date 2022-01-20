#pragma once
#include <string>

#include "ast/Ast.h"
#include "aux/StringInterner.h"

namespace pink {
    class Binop : public Ast {
    public:
        InternedString op;
        Ast*           left;
        Ast*           right;

    Binop(Location& loc, InternedString o, Ast* l, Ast* r);
    virtual ~Binop();

    virtual Ast* Clone() override;

    static bool classof(const Ast* t);

    virtual std::string ToString() override;

    virtual Outcome<Type*, Error> Getype(Environment& env) override;
    // virtual std::variant<pink::Error, llvm::Value*> ToLLVM(Environment& e) = 0;
    };
}
