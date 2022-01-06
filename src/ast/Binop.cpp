#include "ast/Binop.hpp"


namespace pink {
    Binop::Binop(Location& loc, InternedString o, Ast* l, Ast* r)
        : Ast(Ast::Kind::Binop, loc), op(o), left(l), right(r)
    {

    }

    Binop::~Binop()
    {
        delete left;
        delete right;
    }

    Ast* Binop::Clone()
    {
        return new Binop(loc, op, left->Clone(), right->Clone());
    }

    bool Binop::classof(const Ast* t)
    {
        return t->GetKind() == Ast::Kind::Binop;
    }

    std::string Binop::ToString()
    {
        return left->ToString() + " " + std::string(op) + " " + right->ToString();
    }
}
