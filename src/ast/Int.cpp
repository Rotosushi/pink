
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

    std::unique_ptr<Ast> Int::Clone()
    {
        return std::make_unique<Int>(loc, value);
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
    	Type* int_type = env.types.GetIntType();
    	Outcome<Type*, Error> result(int_type);
    	return result;
    }
}
