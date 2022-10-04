#include "ast/Ast.h"


namespace pink {
    Ast::Ast(const Ast::Kind kind, const Location& location)
        : kind(kind), loc(location), type(nullptr)
    {

    }

    auto Ast::getKind() const -> const Ast::Kind&
    {
        return kind;
    }

    auto Ast::GetLoc() const -> const Location&
    {
        return loc;
    }
    
    auto Ast::Getype(const Environment& env) -> Outcome<Type*, Error>
    {
    	if (type != nullptr)
    	{
    		return {type};
    	}

        Outcome<Type*, Error> result = this->GetypeV(env);
        
        if (result)
        {
            type = result.GetFirst();
        }
        
        return result;
    }
}
