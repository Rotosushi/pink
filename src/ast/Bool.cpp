#include "ast/Bool.h"

#include "aux/Environment.h"

namespace pink {
    Bool::Bool(Location l, bool b)
        : Ast(Ast::Kind::Bool, l), value(b)
    {

    }

    Bool::~Bool()
    {

    }

    std::unique_ptr<Ast> Bool::Clone()
    {
        return std::make_unique<Bool>(loc, value);
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
    
    /*
       ---------------------
    	env |- true : Bool 
    	
       ---------------------
    	env |- false : Bool
    */
    Outcome<Type*, Error> Bool::Getype(Environment& env)
    {
    	Type* bool_type = env.types.GetBoolType();
    	Outcome<Type*, Error> result(bool_type);
    	return result;
    }
}
