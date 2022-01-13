
#include "ast/Bind.hpp"

namespace pink {
    Bind::Bind(Location l, InternedString i, Ast* t)
        : Ast(Ast::Kind::Bind, l), symbol(i), term(t)
    {

    }

    /*
        each node assumes ownership of the passed in pointer,
        then, the whole tree owns itself.
    */
    Bind::~Bind()
    {
        delete (term);
    }

    Ast* Bind::Clone()
    {
        return new Bind(loc, symbol, term->Clone());
    }

    bool Bind::classof(const Ast* a)
    {
        return a->getKind() == Ast::Kind::Bind;
    }

    std::string Bind::ToString()
    {
        return std::string(symbol) += " := " + term->ToString();
    }
}
