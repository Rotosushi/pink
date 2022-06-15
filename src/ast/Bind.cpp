
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
    Outcome<Type*, Error> Bind::GetypeV(std::shared_ptr<Environment> env)
    {
    	/*
    		 for the case of a binding statement, which declares a new variable,
    		 we only want to lookup the symbol in the local scope. because then 
    		 we can construct shadowed names. which makes it easier to write 
    		 functions with local variables, because we can reuse the names
    		 already bound in higher scopes, in the lowe scope.
    		 
    		 This is only because of the bind terms construction of a new binding.
    		 Variable references still do recursive lookup, in order to resolve 
    		 the case where a function uses a global name within it's body.
    		 
    		 and this still prevents local and global redefinition of symbols,
    		 it only allows redefinition in the case where the local scope is 
    		 defining a symbol that is defined in an outer scope.
    		 
    		 #TODO add namespace qualifiers into the language, which will then 
    		 		allow local scopes to shadow a name and still have access to the 
    		 		outer name via the fully qualified name. The shadowing then will 
    		 		be allowed by the local bindings getting a different fully qualified 
    		 		symbol than the global name would. all this would be transparent to 
    		 		the programmer, and would only be on a 'opt-in' basis.
    	*/
    	llvm::Optional<std::pair<Type*, llvm::Value*>> bound(env->bindings->LookupLocal(symbol));
    
    	if (!bound.hasValue())
    	{
			Outcome<Type*, Error> term_type = term->Getype(env);
			
			if (term_type)
			{
				// construct a false binding so we can properly 
				// type statements that occur later within the same block.
				env->false_bindings.push_back(symbol);
				
				env->bindings->Bind(symbol, term_type.GetOne(), nullptr); // construct a false binding to type later statements within this same block.
				
				Outcome<Type*, Error> result(term_type.GetOne());
				return result;
			}
			else 
			{
				return term_type;
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
    Outcome<llvm::Value*, Error> Bind::Codegen(std::shared_ptr<Environment> env)
    {
    	llvm::Optional<std::pair<Type*, llvm::Value*>> bound(env->bindings->LookupLocal(symbol));
    
    	if (!bound.hasValue())
    	{
    		// retrieve the type of the term
			Outcome<Type*, Error> term_type_result = term->Getype(env);
			
			if (!term_type_result)
			{
				return Outcome<llvm::Value*, Error>(term_type_result.GetTwo());
			}
			
			// get the llvm representation of the type
			Outcome<llvm::Type*, Error> term_type = term_type_result.GetOne()->Codegen(env);
			
			if (!term_type)
				return Outcome<llvm::Value*, Error>(term_type.GetTwo());
			
			// get the llvm representation of the term's value
			Outcome<llvm::Value*, Error> term_value = term->Codegen(env);
			
			if (!term_value)
				return term_value;
			
			llvm::Value* ptr = nullptr;
			
			if (env->current_function == nullptr) // this is the global scope, construct a global
			{
				// global_value is the address of the value in question, and has to be retrieved directly from the module,
				//	which should be fine, as the ir_builder will also be associated with the same module.
				// in the case of a global variable, the name is bound to the address of 
				// the global variable's location constructed w.r.t. the module.
				ptr = env->module->getOrInsertGlobal(symbol, term_type.GetOne());
				
				llvm::GlobalVariable* global = env->module->getGlobalVariable(symbol);
				
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
			}
			else // this is a local scope, so construct a local binding
			{
				llvm::BasicBlock* insertion_block = &(env->current_function->getEntryBlock());
				llvm::BasicBlock::iterator insertion_point = insertion_block->getFirstInsertionPt();
				
				// create a new builder to not mess with the previous ones insertion point.
				// we might be able to save/restore the point, but what if the previous 
				// builder was in the global scope, and thus had no basic block to insert into?
				llvm::IRBuilder local_builder(insertion_block, insertion_point);
				
				// in the case of a local symbol the name is bound to the address of 
				// the local variables location, constructed w.r.t. the current function.
				ptr = local_builder.CreateAlloca(term_type.GetOne(), 
												   env->data_layout.getAllocaAddrSpace(), 
												   nullptr, 
												   symbol);
				
				// we can store the value into the stack (alloca) at some 
				// point after we construct it. in this case, relative to 
				// where the bind declaration is located syntactically.
				env->builder->CreateStore(term_value.GetOne(), ptr, symbol);
			}
			// we use term_type_result, as that holds a pink::Type*, which is what Bind expects.
			env->bindings->Bind(symbol, term_type_result.GetOne(), ptr); // bind the symbol to the newly created value
			
			return Outcome<llvm::Value*, Error>(term_value); // return the value stored to support nested binds.
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
