#include "ast/Variable.hpp"

/*
    TODO:   Move each file in the header and source directory
    into the appropriate subfolders. src/ast/Int.cpp include/ast/Bool.hpp etc...
    includ/aux/StringInterner.hpp
*/
namespace pink {
    Variable::Variable(Location l, InternedString s)
        : Ast(Ast::Kind::Variable, l), symbol(s)
    {

    }

    Variable::~Variable()
    {

    }

    Ast* Variable::Clone()
    {
        return new Variable(loc, symbol);
    }

    bool Variable::classof(const Ast* a)
    {
        return a->getKind() == Ast::Kind::Variable;
    }

    std::string Variable::ToString()
    {
        return std::string(symbol);
    }

}
