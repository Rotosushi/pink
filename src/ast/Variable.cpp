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
	    if (bound->second == nullptr)
      {
        FatalError("Tried to reference a variable bound to a nullptr!", __FILE__, __LINE__);
        return Outcome<llvm::Value*, Error> (Error());
      }
      else
      {
        Outcome<llvm::Type*, Error> type_result(bound->first->Codegen(env));

        if (!type_result)
          return Outcome<llvm::Value*, Error>(type_result.GetTwo());

        llvm::Type* bound_type = type_result.GetOne();

        // if the variable is bound to a pointer typed variable, we don't want
        // to load it, because we want the pointer that llvm returns to be the 
        // llvm::Value* we return. 
        //
        // However we also don't want to load if we are on the lhs of an
        // assignment expression, because the CreateStore instruction also 
        // requires the pointer that llvm returns as the location being stored
        // into, even though our type system would say that a Memory Location 
        // 'is' an Int, the llvm::Value* an Int type variable is bound to is a 
        // pointer itself. this is fine for both CreateLoad and CreateStore,
        // it is only when we want to use the value within the variable in
        // another expression, such as binop/unop/application expressions
        // that we need to emit a CreateLoad before we return the Variable.
        //
        if (llvm::isa<pink::PointerType>(bound->first) 
         || llvm::isa<llvm::FunctionType>(bound_type)
         || env->flags->OnTheLHSOfAssignment())
        {
          return Outcome<llvm::Value*, Error> (bound->second);
        }
        else 
        {
          return Outcome<llvm::Value*, Error>(env->builder->CreateLoad(bound_type, bound->second, "load"));
        }
      }  
		}
		else 
		{
			Error error(Error::Code::NameNotBoundInScope, loc);
			Outcome<llvm::Value*, Error> result(error);
			return result;
		}
	}
}
