
#include "ast/Int.h"

#include "aux/Environment.h"

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
        return a->getKind() == Ast::Kind::Int;
    }

    std::string Int::ToString()
    {
        return std::to_string(value);
    }
    
    /*
       ----------------------
    	env |- [0-9]+ : Int
    */
    Outcome<Type*, Error> Int::Getype(Environment& env)
    {
    	Outcome<Type*, Error> result(env.types.GetIntType());
    	return result;
    }
}
