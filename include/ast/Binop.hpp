#pragma once
#include <string>

#include "ast/Ast.hpp"
#include "aux/StringInterner.hpp"

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

    // virtual std::variant<pink::Error, pink::Type*> Getype(Environment& e) = 0;
    // virtual std::variant<pink::Error, llvm::Value*> ToLLVM(Environment& e) = 0;
    };
}
