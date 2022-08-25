
#include "ast/Function.h"

#include "aux/Environment.h"

#include "kernel/Cast.h"

#include "llvm/IR/Verifier.h"
#include "llvm/IR/InlineAsm.h"

namespace pink {
		Function::Function(Location l, 
						   InternedString n, 
						   std::vector<std::pair<InternedString, Type*>> a,
						   std::unique_ptr<Ast> b,
						   SymbolTable* p)
			: Ast(Ast::Kind::Function, l), name(n), arguments(a), body(std::move(b)), bindings(std::make_shared<SymbolTable>(p))
		{
		
		}
		
		Function::~Function()
		{
		
		}
		
		std::unique_ptr<Ast> Function::Clone()
		{
			std::unique_ptr<Ast> result = std::make_unique<Function>(loc, name, arguments, body->Clone(), bindings->OuterScope());
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
						
			result += ")";
			
			result += "{";
			
			result += body->ToString();
			
			result += "}";
			
			return result;
		}
		
		/*
					  env |- a0 : T0, a1 : T1, ..., an : Tn, body : Tb
		 -----------------------------------------------------------------------------
			env |- 'fn' name '(' a0: T0, a1: T1, ..., an : Tn ')' '{' body '} : Tb
		*/
		Outcome<Type*, Error> Function::GetypeV(std::shared_ptr<Environment> env)
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
				bindings->Bind(pair.first, pair.second, nullptr); 
			}
			
			// build a new environment around the functions symboltable, such that we have syntactic scoping.
      std::shared_ptr<Environment> inner_env = std::make_shared<Environment>(env, bindings); 
			
			// type the body with respect to the inner_env
			Outcome<Type*, Error> body_result = body->Getype(inner_env);
			
			// remove the false bindings before returning;
			for (auto& pair : arguments)
			{
				bindings->Unbind(pair.first); 
			}

      // also remove any false bindings that any bind 
      // instructions within this functions scope 
      // constructed to declare local variables.
      for (InternedString fbnd : inner_env->false_bindings)
      {
        inner_env->bindings->Unbind(fbnd);
      }

      inner_env->false_bindings.clear();
			
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

			FunctionType* func_ty = env->types->GetFunctionType(body_result.GetOne(), arg_types);
      // okay, so before we return we need to take care of one more thing,
      // making sure the rest of the program knows of the existance of this 
      // function. and how do we do that? using an early binding of the 
      // function itself to just it's type. which is luckily all we know as of
      // right now. then later when we codegen the function body itself we can 
      // rebind to a fully qualified definition, including both the type and 
      // the implementation.
      //
      // the nullptr is the spot where the implementation will go.
      env->bindings->Bind(name, func_ty, nullptr);

      return Outcome<Type*, Error>(func_ty);
		}
		
		Outcome<llvm::Value*, Error> Function::Codegen(std::shared_ptr<Environment> env)
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
															  *env->module);
			
			
			
			// create the entry block of this function.
			llvm::BasicBlock* entryblock = llvm::BasicBlock::Create(*env->context, "entry", function);
			llvm::BasicBlock::iterator entrypoint = entryblock->getFirstInsertionPt();
			
			// set up a local irbuilder to insert instructions into this function.
      std::shared_ptr<llvm::IRBuilder<>> local_builder = std::make_shared<llvm::IRBuilder<>>(entryblock, entrypoint);
			
      // set up the local_environment for this function
      std::shared_ptr<Environment> local_env = std::make_shared<Environment>(env, bindings, local_builder, function); 
			
			std::vector<std::pair<llvm::Value*, llvm::Value*>> arg_ptrs;
			// construct stack space for all arguments to the function 
			for (int i = 0; i < function->arg_size(); i++)
			{
				llvm::Argument* arg = function->getArg(i);
				llvm::Value* arg_ptr = local_builder->CreateAlloca(arg->getType(),
																  local_env->data_layout.getAllocaAddrSpace(),
																  nullptr,
																  arg->getName());
				
				arg_ptrs.emplace_back(arg, arg_ptr);												  
			}
			
			// make sure all instructions that are not Allocas are placed after 
			// all the allocas
			for (int i = 0; i < arg_ptrs.size(); i++)
			{
				local_builder->CreateStore(arg_ptrs[i].first, arg_ptrs[i].second, false);
				
				local_env->bindings->Bind(arguments[i].first, arguments[i].second, arg_ptrs[i].second);
			}
			
			
			// now we can Codegen the body of the function relative to the local environment
			Outcome<llvm::Value*, Error> body_result = body->Codegen(local_env);
			
			if (!body_result)
			{
				return body_result;
			}
       
      if (std::string(name) == "main")
      {
        // construct the following inline assembly for a syscall to exit.
        // #NOTE: this is very non-portable, and will need to be made
        // separately for each release platform and OS choice.
        // #PORTABILITY
        // #TODO: refactor this section into a function call which then
        // dispatches based upon which target language and OS we are emitting
        // for. As of this early version this is an acceptable solution, as 
        // our language only supports x86-64 on Linux.

        // however, in general, inline asm works in llvm by constructing
        // the llvm::InlineAsm node with the correct asm string and constraints 
        // string (specified:
        // llvm.org/docs/LangRef.html#inline-assembler-expressions)
        // and each asm string must then be used as the argument to 
        // a call instruction, this call instruction is how we direct 
        // local variables into the assembly itself. so for this particular 
        // case, we want to pass it the return value of the body of the
        // function.
        //
        // then, the actual assembly we want to emit is:
        //  mov rax, 60
        //  mov rdi, $0
        //  syscall
        //
        // because we are only ever using the asm within a 'call' instruction,
        // each instruction, in and of itself, has a 'function type' that is,
        // a type which describes the arguments to and resulting value type of 
        // the instruction itself.
        // 
        //  mov rax, 60 
        // takes no arguments (void), because the value is immediate, and returns an
        // integer type.
        //
        //  mov rdi, $0
        // takes a single integer as argument, via $0, which indexes the
        // argument list itself, and takes the first available argument.
        // it returns an integer just like the above instruction.
        //
        //  syscall
        // takes no arguments (void), and returns it's result in rax.
        // in this particular case, we are calling exit(), which does 
        // not return into our program.
        //
        

        llvm::Type* int64Ty = local_builder->getInt64Ty();
        llvm::Type* voidTy  = local_builder->getVoidTy();

        std::vector<llvm::Type*> intArgsTy = { int64Ty };
        std::vector<llvm::Type*> noArgsTy  = {};

        llvm::FunctionType* iasm0Ty = llvm::FunctionType::get(int64Ty, noArgsTy, false);
        llvm::FunctionType* iasm1Ty = llvm::FunctionType::get(int64Ty, intArgsTy, false);
        llvm::FunctionType* iasm2Ty = llvm::FunctionType::get(voidTy, noArgsTy, false);
       
        llvm::Value* bodyVal = body_result.GetOne();
        llvm::InlineAsm* iasm1 = nullptr;
        // So, llvm generates smaller instruction sizes when it handles 
        // it's different types, this is perfectly fine, except that 
        // the inline assembly that we generate is not in all cases 
        // compatible with the registers that llvm uses, so either
        // we need to be smarter about the assembly instructions we 
        // emit, and actually know what size register llvm is going 
        // to use, and be able to adapt our instructions to that,
        // or I think we could cast any return expression to an 
        // Integer size before we use it as the argument to this 
        // specific instruction. Since main always has to return an 
        // integer, which means types that can validly be cast to 
        // integers can also be returned.
        //
        // since, however, we are now considering casting, we should factor 
        // the casting itself into it's own function as well, so that we 
        // can use it later to add casting to the language.
        //
        // from the llvm perspective we have many different kinds of casting 
        // which can be done.
        //
        // for instance to cast between our boolean type and our integer type 
        // we have to emit a zext instruction,
        //
        // or if we want to convert between our integer type and our boolean
        // type we have to emit a trunc instruction.
        //
        // or if we want to convert between the different sizes of possible 
        // integers we can use zext to cast smaller to bigger and 
        // trunc to cast bigger to smaller
        //
        // or if we want to cast between our pointer types, we emit the 
        // bitcast instruction,
        //
        // of if we want to cast between addressspaces for a given pointer type 
        // we emit the addrspacecast instruction,
        //
        // or if we want to cast between an integer and a float we emit the 
        // sitofp instruction,
        //
        // or if we want to cast between a float and an integer we emit the 
        // fptosi instruction,
        //
        // or if we want to cast between a float and an unsigned integer we
        // emit the fptoui instruction,
        //
        // or if we want to cast between an unsigned integer and a float we 
        // emit the uitofp instruction
        //
        //
        // we should also factor this part of Function::Codegen into it's own
        // subroutine, so Function::Codegen can just call something passing
        // in the value to be returned. Maybe EmitMainReturn(llvm::Value*,
        // std::shared_ptr<Environment>)
        // 
        // if the body of main is returning a constantInt that needs a slightly 
        // different inline assembly expression from returning the result of an
        // expression.
        if ( llvm::isa<llvm::ConstantInt>(bodyVal) )
        {
          if (!llvm::InlineAsm::Verify(iasm1Ty, "={rdi},i"))
          {
            FatalError("constraint code for iasm1Ty not valid", __FILE__, __LINE__);
          }
         
          iasm1 = llvm::InlineAsm::get(iasm1Ty, 
                                       "mov rdi, $1",
                                       "={rdi},i", // this says the instruction writes an argument, 
                                                   // which is an immediate integer, to rdi
                                       true,  // hasSideEffects
                                       false, // isAlignStack
                                       llvm::InlineAsm::AsmDialect::AD_Intel,
                                       false); // canThrow

        }
        else // we assume since main passed typechecking that the body_result is in fact 
             // type Int, it's simply held within a register because of the
             // actual shape of the body.  
        {
          if (!llvm::InlineAsm::Verify(iasm1Ty, "={rdi},r"))
          {
            FatalError("constraint code for iasm1Ty not valid", __FILE__, __LINE__);
          }

          
          iasm1 = llvm::InlineAsm::get(iasm1Ty, 
                                       "mov rdi, $1",
                                       "={rdi},r", // this says the instruction writes an argument, 
                                                   // which is a register, to rdi
                                       true,  // hasSideEffects
                                       false, // isAlignStack
                                       llvm::InlineAsm::AsmDialect::AD_Intel,
                                       false); // canThrow


        }


        if (!llvm::InlineAsm::Verify(iasm0Ty, "={rax}"))
        {
          FatalError("constraint code for iasm0Ty not valid", __FILE__, __LINE__);
        }

        if (!llvm::InlineAsm::Verify(iasm2Ty, ""))
        {
          FatalError("constraint code for iasm2Ty not valid", __FILE__, __LINE__);
        }
        

        llvm::InlineAsm* iasm0 = llvm::InlineAsm::get(iasm0Ty, 
                                                      "mov rax, 60",
                                                      "={rax}", // this says the instruction writes an immediate int to rax
                                                      true,  // hasSideEffects
                                                      false, // isAlignStack
                                                      llvm::InlineAsm::AsmDialect::AD_Intel,
                                                      false); // canThrow
         

        llvm::InlineAsm* iasm2 = llvm::InlineAsm::get(iasm2Ty, 
                                                      "syscall",
                                                      "", // syscall uses no data, and does not return in this case, 
                                                          // other times it's return value is within rax.
                                                      true,  // hasSideEffect
                                                      false, // isAlignStack
                                                      llvm::InlineAsm::AsmDialect::AD_Intel,
                                                      false); // canThrow 
        // initally the numbering followed the order we emitted, however
        // rax is a commonly selected register for use by the compiler,
        // and as such it selects rax as the place to store the results
        // of common expressions, this causes us to overwrite the correct 
        // return code when we load the rax register with the exit code 
        // if we emit a call to iasm1 after emitting the call to 
        // iasm0. hence the reversal.
        Outcome<llvm::Value*, Error> cast_result = Cast(bodyVal, env->builder->getInt64Ty(), local_env);

        if (!cast_result)
          return cast_result;

        std::vector<llvm::Value*> iasm1Args = { cast_result.GetOne() };

        local_builder->CreateCall(iasm1Ty, iasm1, iasm1Args);

        
        local_builder->CreateCall(iasm0Ty, iasm0);
        

        local_builder->CreateCall(iasm2Ty, iasm2);
       
        // #NOTE: #TODO:
        // if this line is not here, the program segfaults when 
        // we try and print the program to a file. I am really unsure 
        // why this is, and the segfault happens deep in llvm.
        // so, figure out why this happens and what we can do to 
        // remove this line. this is in fact always dead code,
        // so i am initailly unsure why this is even necessary.
        local_builder->CreateRet(bodyVal);
      }
      else
      {
			  // use the value returned by the body to construct a return statement 
			  local_builder->CreateRet(body_result.GetOne());
      }

			// call verifyFunction to validate the generated code for consistency 
			llvm::verifyFunction(*function);
			
      // we now need to fill in the binding of this function with it's
      // definition. so first we erase the incomplete definition, and 
      // then create the full definition.
      env->bindings->Unbind(name);
      env->bindings->Bind(name, getype_result.GetOne(), function);

			// return the function as the result of code generation
			return Outcome<llvm::Value*, Error>(function);
		}
}


