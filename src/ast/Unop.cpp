#include "ast/Unop.h"

namespace pink {
    Unop::Unop(Location& loc, InternedString o, Ast* r)
        : Ast(Ast::Kind::Unop, loc), op(o), right(r)
    {

    }

    Unop::~Unop()
    {

    }

    Ast* Unop::Clone()
    {
        return new Unop(loc, op, right->Clone());
    }

    bool Unop::classof(const Ast* t)
    {
        return t->getKind() == Ast::Kind::Unop;
    }

    std::string Unop::ToString()
    {
        return std::string(op) + right->ToString();
    }
}
