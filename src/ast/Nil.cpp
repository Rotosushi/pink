#include "ast/Nil.h"

#include "aux/Environment.h"

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
        return a->getKind() == Ast::Kind::Nil;
    }

    std::string Nil::ToString()
    {
        return std::string("nil");
    }
    
    /*
    	--------------------
    	  env |- nil : Nil
    */
    Outcome<Type*, Error> Nil::Getype(Environment& env)
    {
    	Outcome<Type*, Error> result(env.types.GetNilType());
    	return result;
    }
}
