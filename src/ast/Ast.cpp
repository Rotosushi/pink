#include "ast/Ast.hpp"


namespace pink {
    Ast::Ast(const Ast::Kind k, Location l)
        : kind(k), loc(l)
    {

    }

    Ast::~Ast()
    {

    }

    Ast::Kind Ast::getKind() const
    {
        return kind;
    }

    Location Ast::GetLoc() const
    {
        return loc;
    }
}
