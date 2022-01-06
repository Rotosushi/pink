
#include "ast/Int.hpp"

namespace pink {
    Int::Int(Location l, int i)
        : Ast(Ast::Kind::Int, l), value(i)
    {

    }

    Int::~Int()
    {

    }

    Ast* Int::Clone()
    {
        return new Int(loc, value);
    }

    bool Int::classof(const Ast* a)
    {
        return a->GetKind() == Ast::Kind::Int;
    }

    std::string Int::ToString()
    {
        return std::to_string(value);
    }
}
