#include "ast/Nil.hpp"

namespace pink {
    Nil::Nil(Location l)
        : Ast(Ast::Kind::Nil, l)
    {

    }

    Nil::~Nil()
    {

    }

    Ast* Nil::Clone()
    {
        return new Nil(loc);
    }

    bool Nil::classof(const Ast* a)
    {
        return a->GetKind() == Ast::Kind::Nil;
    }

    std::string Nil::ToString()
    {
        return std::string("nil");
    }
}
