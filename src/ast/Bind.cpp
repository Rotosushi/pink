
#include "ast/Bind.h"

#include "aux/Environment.h"

namespace pink {
    Bind::Bind(Location l, InternedString i, std::unique_ptr<Ast> t)
        : Ast(Ast::Kind::Bind, l), symbol(i), term(std::move(t))
    {

    }

    /*
        each node assumes ownership of the passed in pointer,
        then, the whole tree owns itself.
    */
    Bind::~Bind()
    {
    
    }

    std::unique_ptr<Ast> Bind::Clone()
    {
        return std::make_unique<Bind>(loc, symbol, term->Clone());
    }

    bool Bind::classof(const Ast* a)
    {
        return a->getKind() == Ast::Kind::Bind;
    }

    std::string Bind::ToString()
    {
        return std::string(symbol) += " := " + term->ToString();
    }
    
    /*
    	env |- t : T, x is-not-in env.bindings
    	---------------------------------------
    		  env |- x := t : T, x : T
    		  
    	note: Typeing a binding expression is distinct 
    		from evaluating it. and it is only when we 
    		have both type and value that we can construct 
    		a binding in the symboltable.
    */
    Outcome<Type*, Error> Bind::Getype(Environment& env)
    {
    	llvm::Optional<std::pair<Type*, llvm::Value*>> bound(env.bindings.Lookup(symbol));
    
    	if (!bound.hasValue())
    	{
			Outcome<Type*, Error> term_result = term->Getype(env);
			
			if (term_result)
			{
				Outcome<Type*, Error> result(term_result.GetOne());
				return result;
			}
			else 
			{
				return term_result;
			}
		}
		else 
		{
			// #TODO: if the binding term has the same type as what the symbol 
			//  is already bound to, we could treat the binding as equivalent 
			//  to an assignment here.
			Error error(Error::Kind::Type, 
					  std::string("[") + symbol + std::string("] is already bound to [")
					  	+ bound->first->ToString() + std::string("]"),
					  loc);
			Outcome<Type*, Error> result(error);
			return result;
		}
    }
}
