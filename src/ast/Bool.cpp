#include "ast/Bool.hpp"

namespace pink {
    Bool::Bool(Location l, bool b)
        : Ast(Ast::Kind::Bool, l), value(b)
    {

    }

    Bool::~Bool()
    {

    }

    Ast* Bool::Clone()
    {
        return new Bool(loc, value);
    }

    bool Bool::classof(const Ast* a)
    {
        return a->getKind() == Ast::Kind::Bool;
    }

    std::string Bool::ToString()
    {
        if (value == true)
            return std::string("true");
        else
            return std::string("false");
    }
}
