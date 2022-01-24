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

    std::unique_ptr<Ast> Nil::Clone()
    {
        return std::make_unique<Nil>(loc);
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
    	Type* nil_type = env.types.GetNilType();
    	Outcome<Type*, Error> result(nil_type);
    	return result;
    }
}
