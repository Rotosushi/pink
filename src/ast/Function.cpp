
#include "ast/Function.h"
#include "aux/Environment.h"

#include "llvm/IR/Verifier.h"


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
			
			result += std::string("fn ") + name;
			
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
		Outcome<Type*, Error> Function::GetypeV(Environment& env)
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
			/*
				first, get the functions prototype/declaration from the module
				
				set the env.current_fn to this function.
				
				set up a new local env relative to the local scope of this function.
				
				then, construct the first basic block within the function 
				and set the ir_builder's insertion point to that.
				
				then, Codegen the body of the function.
				
				using the value returned from Codegen'ing the body, construct a return statement.
				
				return the function itself as the result of Codegen.
			*/
			Outcome<Type*, Error> getype_result = this->Getype(env);
			
			if (!getype_result)
			{
				return Outcome<llvm::Value*, Error>(getype_result.GetTwo());
			}
			
			Outcome<llvm::Type*, Error> ty_codegen_result = getype_result.GetOne()->Codegen(env);
			
			if (!ty_codegen_result)
			{
				return Outcome<llvm::Value*, Error>(ty_codegen_result.GetTwo());
			}
			
			llvm::FunctionType* function_ty = llvm::cast<llvm::FunctionType>(ty_codegen_result.GetOne());
			
			llvm::Function* function = llvm::Function::Create(function_ty, 
															  llvm::Function::ExternalLinkage, 
															  name, 
															  env.module);
			
			
			
			// create the entry block of this function.
			llvm::BasicBlock* entryblock = llvm::BasicBlock::Create(env.context, "entry", function);
			llvm::BasicBlock::iterator entrypoint = entryblock->getFirstInsertionPt();
			
			// set up a local irbuilder to insert instructions into this function.
			llvm::IRBuilder local_builder(entryblock, entrypoint);
			
			Environment local_env(env, bindings, local_builder, function); // set up the local_environment for this function
			
			std::vector<std::pair<llvm::Value*, llvm::Value*>> arg_ptrs;
			// construct stack space for all arguments to the function 
			for (int i = 0; i < function->arg_size(); i++)
			{
				llvm::Argument* arg = function->getArg(i);
				llvm::Value* arg_ptr = local_builder.CreateAlloca(arg->getType(),
																  local_env.data_layout.getAllocaAddrSpace(),
																  nullptr,
																  arg->getName());
				
				arg_ptrs.emplace_back(arg, arg_ptr);												  
			}
			
			// make sure all instructions that are not Allocas are placed after 
			// all the allocas
			for (int i = 0; i < arg_ptrs.size(); i++)
			{
				local_builder.CreateStore(arg_ptrs[i].first, arg_ptrs[i].second, false);
				
				local_env.bindings.Bind(arguments[i].first, arguments[i].second, arg_ptrs[i].second);
			}
			
			
			// now we can Codegen the body of the function relative to the local environment
			Outcome<llvm::Value*, Error> body_result = body->Codegen(local_env);
			
			if (!body_result)
			{
				return body_result;
			}
			
			// use the value returned by the body to construct a return statement 
			local_builder.CreateRet(body_result.GetOne());
			
			// call verifyFunction to validate the generated code for consistency 
			llvm::verifyFunction(*function);
			
			// return the function as the result of code generation
			return Outcome<llvm::Value*, Error>(function);
		}
}


