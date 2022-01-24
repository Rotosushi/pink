#include "ast/Variable.h"
#include "aux/Environment.h"

namespace pink {
    Variable::Variable(Location l, InternedString s)
        : Ast(Ast::Kind::Variable, l), symbol(s)
    {

    }

    Variable::~Variable()
    {

    }

    std::unique_ptr<Ast> Variable::Clone()
    {
        return std::make_unique<Variable>(loc, symbol);
    }

    bool Variable::classof(const Ast* a)
    {
        return a->getKind() == Ast::Kind::Variable;
    }

    std::string Variable::ToString()
    {
        return std::string(symbol);
    }

	/*
		env |- x is-in env, x : T
	  -----------------------------
			  env |- x : T
	*/
	Outcome<Type*, Error> Variable::Getype(Environment& env)
	{
		llvm::Optional<std::pair<Type*, llvm::Value*>> bound = env.bindings.Lookup(symbol);
		
		if (bound.hasValue())
		{
			Outcome<Type*, Error> result(bound->first);
			return result;
		}
		else 
		{
			Error error(Error::Kind::Type, std::string("[") + symbol + std::string("] not bound in environment"), loc);
			Outcome<Type*, Error> result(error);
			return result;
		}
	}
}
