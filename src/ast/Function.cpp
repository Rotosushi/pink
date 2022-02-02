
#include "ast/Function.h"
#include "aux/Environment.h"


namespace pink {
		Function::Function(Location l, 
						   InternedString n, 
						   std::vector<std::pair<InternedString, Type*>> a,
						   std::unique_ptr<Ast> b,
						   SymbolTable* p)
			: Ast(Ast::Kind::Function, l), name(n), arguments(a), body(std::move(b)), bindings(p)
		{
		
		}
		
		Function::~Function()
		{
		
		}
		
		std::unique_ptr<Ast> Function::Clone()
		{
			std::unique_ptr<Ast> result = std::make_unique<Function>(loc, name, arguments, body->Clone(), bindings.OuterScope());
			return result;
		}
		
		bool Function::classof(const Ast* a)
		{
			return a->getKind() == Ast::Kind::Function;
		}
		
		/*
			'fn' name '(' (args[0].first ':' args[0].second->ToString() (',')?)+ ')' '\n'
			'{' '\n'
				body->ToString() '\n'
			'}' '\n'
		*/
		std::string Function::ToString()
		{
			std::string result;
			
			result += std::string("fn") + name;
			
			result += " (";
			
			int len = arguments.size();
			
			for (int i = 0; i < len; i++)
			{
				result += arguments[i].first;
				result += ": " + arguments[i].second->ToString();
				
				if (i < (len - 1))
					result += ", ";
			}
						
			result += ")\n";
			
			result += "{\n";
			
			result += body->ToString();
			
			result += "}\n";
			
			return result;
		}
		
		/*
					  env |- a0 : T0, a1 : T1, ..., an : Tn, body : Tb
		 -----------------------------------------------------------------------------
			env |- 'fn' name '(' a0: T0, a1: T1, ..., an : Tn ')' '{' body '} : Tb
		*/
		Outcome<Type*, Error> Function::Getype(Environment& env)
		{
			// first, construct a bunch of false bindings with the correct Types.
			// such that we can properly Type the body, which presumably uses said bindings.
			
			// next, construct a new environment, where the SymbolTable is now the 
			// SymbolTable of this function, such that Lookup will resolve the new 
			// bindings constructed
			
			// then type the body of the function.
			
			// even if the body returned an error, first, clean up the false bindings
			// so that codegen can properly bind the arguments to their actual llvm::Value*s
			
			// construct a Function Type* from the types of the 
			// arguments and the type we got from the body.
			
			// construct the false bindings, with no real llvm::Value*
			for (auto& pair : arguments)
			{
				bindings.Bind(pair.first, pair.second, nullptr); 
			}
			
			// build a new environment around the functions symboltable, such that we have syntactic scoping.
			Environment inner_env(env, bindings); 
			
			// type the body with respect to the inner_env
			Outcome<Type*, Error> body_result = body->Getype(inner_env);
			
			// remove the false bindings before returning;
			for (auto& pair : arguments)
			{
				bindings.Unbind(pair.first); 
			}
			
			if (!body_result)
			{
				return body_result;
			}
			
			// build the function type to return it.
			std::vector<Type*> arg_types;
			
			for (auto& pair : arguments)
			{
				arg_types.emplace_back(pair.second);
			}
			
			FunctionType* func_ty = env.types.GetFunctionType(body_result.GetOne(), arg_types);
			return Outcome<Type*, Error>(func_ty);
		}
		
		Outcome<llvm::Value*, Error> Function::Codegen(Environment& env)
		{
		
		}
}


