#include "ast/Ast.h"


namespace pink {
    Ast::Ast(const Ast::Kind k, Location l)
        : kind(k), loc(l), type(nullptr)
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
    
    Outcome<Type*, Error> Ast::Getype(Environment& env)
    {
    	if (type)
    	{
    		return Outcome<Type*, Error>(type);
    	}
    	else 
    	{
    		Outcome<Type*, Error> result = this->GetypeV(env);
    		
    		if (result)
    		{
    			type = result.GetOne();
    		}
    		
    		return result;
    	}
    }
}
