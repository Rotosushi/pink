#pragma once
#include <string>

#include "ast/Ast.h"
#include "aux/StringInterner.h"

namespace pink {
    class Unop : public Ast {
    public:
        InternedString op;
        Ast* right;

    Unop(Location& loc, InternedString o, Ast* r);
    virtual ~Unop();

    virtual Ast* Clone() override;

    static bool classof(const Ast* t);

    virtual std::string ToString() override;
    
    virtual Outcome<Type*, Error> Getype(Environment& env) override;
    };
}
