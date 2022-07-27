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
	Outcome<Type*, Error> Variable::GetypeV(std::shared_ptr<Environment> env)
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
	
	Outcome<llvm::Value*, Error> Variable::Codegen(std::shared_ptr<Environment> env)
	{
		llvm::Optional<std::pair<Type*, llvm::Value*>> bound = env->bindings->Lookup(symbol);
		
		if (bound.hasValue())
		{
			Outcome<llvm::Type*, Error> llvm_type_result = bound->first->Codegen(env);
			
			if (!llvm_type_result)
			{
				return Outcome<llvm::Value*, Error>(llvm_type_result.GetTwo());
			}
			
			llvm::LoadInst* loaded_value = env->builder->CreateLoad(llvm_type_result.GetOne(), bound->second);
			
			Outcome<llvm::Value*, Error> result(loaded_value);
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
