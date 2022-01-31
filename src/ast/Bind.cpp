
#include "ast/Bind.h"

#include "aux/Environment.h"

#include "llvm/IR/GlobalVariable.h"

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
			//  to an assignment instead of an error. different types would still be an error.
			Error error(Error::Kind::Type, 
					  std::string("[") + symbol + std::string("] is already bound to [")
					  	+ bound->first->ToString() + std::string("]"),
					  loc);
			Outcome<Type*, Error> result(error);
			return result;
		}
    }
    
    
    /*
    	constructs a new variable with the name associated with the bind expression.
    	if the current symboltable in the global scope, then we construct a global 
    	variable. once functions are added to the language then if the symboltable 
    	is associated with a function, i.e. is a local scope, then we allocate the 
    	new variable on the stack by constructing an alloca.
    */
    Outcome<llvm::Value*, Error> Bind::Codegen(Environment& env)
    {
    	llvm::Optional<std::pair<Type*, llvm::Value*>> bound(env.bindings.Lookup(symbol));
    
    	if (!bound.hasValue())
    	{
			Outcome<Type*, Error> term_type_result = term->Getype(env);
			
			if (!term_type_result)
			{
				return Outcome<llvm::Value*, Error>(term_type_result.GetTwo());
			}
			
			Outcome<llvm::Type*, Error> term_type = term_type_result.GetOne()->Codegen(env);
			
			if (!term_type)
				return Outcome<llvm::Value*, Error>(term_type.GetTwo());
			
			Outcome<llvm::Value*, Error> term_value = term->Codegen(env);
			
			if (!term_value)
				return term_value;
			
			// #TODO: after implementing local scopes, check for this scope being local/global 
			//			and construct different kinds of variables accordingly.
			//			if(env.bindings.GetParentScope() == nullptr) -> means symboltable is the global scope.
			// global_value is the address of the value in question, and has to be retrieved directly from the module,
			//	which should be fine, as the ir_builder will also be associated with the same module.
			llvm::Constant* global_value = env.module.getOrInsertGlobal(symbol, term_type.GetOne());
			//llvm::GlobalVariable* global_value = new llvm::GlobalVariable(env.module, 
			//									term_type.GetOne(), /* llvm::Type* */
			//									false, /* isConstant */
			//									llvm::GlobalValue::CommonLinkage);
			
			llvm::GlobalVariable* global = env.module.getGlobalVariable(symbol);
			
			if (llvm::isa<llvm::Constant>(term_value.GetOne()))
			{
				llvm::Constant* constant = llvm::dyn_cast<llvm::Constant>(term_value.GetOne());
				global->setInitializer(constant);
			}
			else 
			{
				Error error(Error::Kind::Semantic, "Global values must have constant initializers.", term->GetLoc());
				return Outcome<llvm::Value*, Error>(error);
			}
			
			return Outcome<llvm::Value*, Error>(global_value); // return the value stored to support nested binds.
		}
		else 
		{
			// #TODO: if the binding term has the same type as what the symbol 
			//  is already bound to, we could treat the binding as equivalent 
			//  to an assignment here.
			Error error(Error::Kind::Semantic, 
					  std::string("[") + symbol + std::string("] is already bound to [")
					  	+ bound->first->ToString() + std::string("]"),
					  loc);
			Outcome<llvm::Value*, Error> result(error);
			return result;
		}
    }
}
