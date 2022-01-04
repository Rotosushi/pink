#include "Nil.hpp"

namespace pink {
    Nil::Nil(Location l)
        : Ast(Ast::Kind::Nil, l)
    {

    }

    std::string Nil::ToString()
    {
        return std::string("nil");
    }

    std::ostream& Nil::Print(std::ostream& out)
    {
        return out << "nil";
    }
}
