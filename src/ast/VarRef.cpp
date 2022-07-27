#include "ast/VarRef.h"
#include "aux/Environment.h"


namespace pink {
    VarRef::VarRef(Location l, InternedString s)
        : Ast(Ast::Kind::VarRef, l), symbol(s)
    {

    }

    VarRef::~VarRef()
    {

    }

    std::unique_ptr<Ast> VarRef::Clone()
    {
        return std::make_unique<VarRef>(loc, symbol);
    }

    bool VarRef::classof(const Ast* a)
    {
        return a->getKind() == Ast::Kind::VarRef;
    }

    std::string VarRef::ToString()
    {
        return std::string(symbol);
    }

	/*
		env |- x is-in env, x : T
	  -----------------------------
			  env |- x : T
	*/
	Outcome<Type*, Error> VarRef::GetypeV(std::shared_ptr<Environment> env)
	{
		llvm::Optional<std::pair<Type*, llvm::Value*>> bound = env->bindings->Lookup(symbol);
		
		if (bound.hasValue())
		{
			Outcome<Type*, Error> result(bound->first);
			return result;
		}
		else 
		{
			Error error(Error::Code::NameNotBoundInScope, loc);
			Outcome<Type*, Error> result(error);
			return result;
		}
	}
	
	Outcome<llvm::Value*, Error> VarRef::Codegen(std::shared_ptr<Environment> env)
	{
		llvm::Optional<std::pair<Type*, llvm::Value*>> bound = env->bindings->Lookup(symbol);
		
		if (bound.hasValue())
		{			
			Outcome<llvm::Value*, Error> result(bound->second);
			return result;
		}
		else 
		{
			Error error(Error::Code::NameNotBoundInScope, loc);
			Outcome<llvm::Value*, Error> result(error);
			return result;
		}
	}
}
