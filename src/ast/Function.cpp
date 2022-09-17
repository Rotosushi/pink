
#include "ast/Function.h"

#include "aux/Environment.h"

#include "kernel/Cast.h"
#include "kernel/StoreAggregate.h"

#include "support/LLVMErrorToString.h"

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
			env |- 'fn' name '(' a0: T0, a1: T1, ..., an : Tn ')' '{' body '} : T0 -> T1 -> ... -> Tn -> Tb

      If a function takes an argument which is not a single value type, then we need to 
      mark that parameter as byval(<ty>). where <ty> is the pointee type,
      additionalily, any non single value type must 
      then be passed via a pointer in that parameter, which means promoting the type to 
      that of a pointer within the argument.

      If a function returns an object which is not a sigle value type, then we need to 
      modify the underlying function type to return void, and add an extra parameter 
      which has type, pointer to the non single value type, and which has the parameter 
      attribute 'sret(<ty>)' where <ty> is the pointee type.

      So, I don't think that a pink::FunctionType needs to be modified in any way, just 
      it's mapping to a llvm::FunctionType, however call sites must also be aware of 
      the difference, because the sret(<ty>) parameter must be allocated by the caller
      before we call the function.
    */
		Outcome<Type*, Error> Function::GetypeV(const Environment& env)
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
      std::unique_ptr<Environment> inner_env = std::make_unique<Environment>(env, bindings); 
			
			// type the body with respect to the inner_env
			Outcome<Type*, Error> body_result = body->Getype(*inner_env);
			
			// remove the false bindings before returning;
			for (auto& pair : arguments)
			{
				bindings->Unbind(pair.first); 
			}

      // also remove any false bindings that any bind 
      // instructions within this functions scope 
      // constructed to declare local variables.
      for (InternedString fbnd : *inner_env->false_bindings)
      {
        inner_env->bindings->Unbind(fbnd);
      }

      inner_env->false_bindings->clear();
			
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

			FunctionType* func_ty = env.types->GetFunctionType(body_result.GetFirst(), arg_types);
      // okay, so before we return we need to take care of one more thing,
      // making sure the rest of the program knows of the existance of this 
      // function. and how do we do that? using an early binding of the 
      // function itself to just it's type. which is luckily all we know as of
      // right now. then later when we codegen the function body itself we can 
      // rebind to a fully qualified definition, including both the type and 
      // the implementation.
      //
      // the nullptr is the spot where the implementation will go.
      env.bindings->Bind(name, func_ty, nullptr);

      return Outcome<Type*, Error>(func_ty);
		}
		
		Outcome<llvm::Value*, Error> Function::Codegen(const Environment& env)
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
				return Outcome<llvm::Value*, Error>(getype_result.GetSecond());
			}
			
			Outcome<llvm::Type*, Error> ty_codegen_result = getype_result.GetFirst()->Codegen(env);
			
			if (!ty_codegen_result)
			{
				return Outcome<llvm::Value*, Error>(ty_codegen_result.GetSecond());
			}

      /*
       *  if the function takes an argument whose size is greater than a single
       *  value, we must add the byval(<ty>) parameter attribute to that parameter.
       *  and the parameter must be promoted to a pointer. luckily
       *  FunctionType::Codegen promotes such parameters to pointers already,
       *  so all we must do here is add the byval(<ty>) attribute
       *
       *  if the functions return type is larger than a single value, then 
       *  FunctionType::Codegen already promoted it to the first argument of
       *  the llvm::FunctionType. this parameter needs to recieve the
       *  sret(<ty>) parameter attribute.
       *  and applications of said function must emit an allocation for the
       *  return value.
       * 
       */
      pink::FunctionType* p_fn_ty = llvm::cast<pink::FunctionType>(getype_result.GetFirst());

      Outcome<llvm::Type*, Error> p_fn_ret_ty_result(Error(Error::Code::None, Location()));
      llvm::Type* fn_ret_ty = nullptr;
      llvm::FunctionType* function_ty = nullptr;
      llvm::Function* function = nullptr;

      // main return's void from llvm's perspective, 
      // because main returns via a syscall.
      if (strcmp(name, "main") == 0)
      {
        pink::FunctionType* main_fn_ty = env.types->GetFunctionType(env.types->GetVoidType(), p_fn_ty->arguments);
        
        Outcome<llvm::Type*, Error> main_fn_ty_result = main_fn_ty->Codegen(env);

        if (!main_fn_ty_result)
          return Outcome<llvm::Value*, Error>(main_fn_ty_result.GetSecond());

        function_ty = llvm::cast<llvm::FunctionType>(main_fn_ty_result.GetFirst());
        
        fn_ret_ty = function_ty->getReturnType();        

        function = llvm::Function::Create(function_ty,
                                          llvm::Function::ExternalLinkage,
                                          name,
                                          *env.module); 
      }
      else
      {		  
        p_fn_ret_ty_result = p_fn_ty->result->Codegen(env);

        if (!p_fn_ret_ty_result)
          return Outcome<llvm::Value*, Error>(p_fn_ret_ty_result.GetSecond());

        fn_ret_ty = p_fn_ret_ty_result.GetFirst();

			  function_ty = llvm::cast<llvm::FunctionType>(ty_codegen_result.GetFirst());
			
			  function = llvm::Function::Create(function_ty, 
				  		 									          llvm::Function::ExternalLinkage, 
					  										          name, 
						  									          *env.module);
      }
      /*
       *  okay okay. the function itself holds it's own Attributes, that makes
       *  sense. It also provides a limited API for adding and removing
       *  attributes from three main attribute categories; function attributes, 
       *  return value attributes, and parameter attributes.
       *
       *
       *
       */
      llvm::AttrBuilder ret_attr_builder(*env.context);
      /* find out if we need to
       * add the sret parameter attribute to a parameter 
       * of the function.
       *
       * this is within a conditional because if it true, then 
       * the parameter list holds the return value, so the parameter 
       * list is one element larger that the pink::FunctionType says.
       * so we have to use different offsets to get to each argument.
       */
     
      if (!fn_ret_ty->isSingleValueType() && function_ty->getReturnType()->isVoidTy())
      {
        // since the return type is not a single value type, we know that 
        // the function type associated with this function will have an 
        // actual return type of void, and the first parameter will be the 
        // return value of this function, knowing that, we then need to add 
        // the sret(<ty>) attribute to this parameter attribute
        ret_attr_builder.addStructRetAttr(fn_ret_ty);
        function->addParamAttrs(0, ret_attr_builder);
        /*
         *  Now all we need to do is add any byval(<ty>) attributes 
         *  to any argument which needs it. We must be careful to 
         *  mention however, that in the translation from pink::FunctionType
         *  to llvm::FunctionType we also promote any non singleValueType to 
         *  an opaque pointer type. So we have to use the arguments within the 
         *  Function to know the type. as otherwise we couldn't tell the
         *  difference between a pointer that needed the byval(<ty>) attribute,
         *  and a pointer that did not..
         */
        for (size_t i = 1; i < p_fn_ty->arguments.size(); i++)
        {
          llvm::AttrBuilder param_attr_builder(*env.context);
          
          Outcome<llvm::Type*, Error> param_ty_codegen_result = p_fn_ty->arguments[i]->Codegen(env);

          if (!param_ty_codegen_result)
            return Outcome<llvm::Value*, Error>(param_ty_codegen_result.GetSecond());

          llvm::Type* param_ty = param_ty_codegen_result.GetFirst();

          // since this type is not a single value type, this parameter needs
          // the byval(<ty>) parameter attribute
          if (!param_ty->isSingleValueType())
          {
            param_attr_builder.addByValAttr(param_ty);
            function->addParamAttrs(i, param_attr_builder);
          }
        }
      }
      else
      {
        /*
         *  all we need to do here is add any byval(<ty>) attributes 
         *  to any argument which needs it.
         */
        for (size_t i = 0; i < p_fn_ty->arguments.size(); i++)
        {
          llvm::AttrBuilder param_attr_builder(*env.context);
          
          Outcome<llvm::Type*, Error> param_ty_codegen_result = p_fn_ty->arguments[i]->Codegen(env);

          if (!param_ty_codegen_result)
            return Outcome<llvm::Value*, Error>(param_ty_codegen_result.GetSecond());

          llvm::Type* param_ty = param_ty_codegen_result.GetFirst();

          // since this type is not a single value type, this parameter needs
          // the byval(<ty>) parameter attribute
          if (!param_ty->isSingleValueType())
          {
            param_attr_builder.addByValAttr(param_ty);
            function->addParamAttrs(i, param_attr_builder);
          }
        }
      }
			
			
			
			// create the entry block of this function.
			llvm::BasicBlock* entryblock = llvm::BasicBlock::Create(*env.context, "entry", function);
			llvm::BasicBlock::iterator entrypoint = entryblock->getFirstInsertionPt();
			
			// set up a local irbuilder to insert instructions into this function.
      std::shared_ptr<llvm::IRBuilder<>> local_builder = std::make_shared<llvm::IRBuilder<>>(entryblock, entrypoint);
			
      // set up the local_environment for this function
      std::unique_ptr<Environment> local_env = std::make_unique<Environment>(env, bindings, local_builder, function); 
			
			std::vector<std::pair<llvm::Value*, llvm::Value*>> arg_ptrs;
			// construct stack space for all arguments to the function 
			for (size_t i = 0; i < function->arg_size(); i++)
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
			for (size_t i = 0; i < arguments.size(); i++)
			{
        //Outcome<llvm::Type*, Error> param_ty_result = p_fn_ty->arguments[i]->Codegen(env);

        //if (!param_ty_result)
        //  return Outcome<llvm::Value*, Error>(param_ty_result.GetTwo());

        //llvm::Type* arg_ty = param_ty_result.GetFirst();
        llvm::Argument* arg = function->getArg(i);
        
        llvm::Type* arg_ty = arg->getType();

        if (arg_ty->isSingleValueType())
        {
				  local_builder->CreateStore(arg_ptrs[i].first, arg_ptrs[i].second, false);
        }
        else
        {
          StoreAggregate(arg_ty, arg_ptrs[i].second, arg_ptrs[i].first, env);
        }
        // if we are within the body of a function which returns a type that is 
        // larger than a single value type, we modify the function type of said 
        // function to take an extra argument which is the return value alloca ptr.
        // however in this case, arg_ptrs.size() will be greater than the size
        // of the arguments from the perspective of our function type. so we
        // need some way of associating the return value alloca argument within
        // the body of the function. 
				local_env->bindings->Bind(this->arguments[i].first, p_fn_ty->arguments[i], arg_ptrs[i].second);
			}
		
      // So, we have an error with GEP again, an assertion failure of
      // isOpaqueOrPointeeTypeMatches. Now historically this error has occured 
      // when the Type in memory we pass to the GEP instruction is of pointer
      // type instead of the pointee type. so i suspect the same has occured
      // here. The error probably occurs when we bind the argument to it's
      // passed in value. because we modify the argument of struct type to be 
      // a pointer instead. now, this is fine from a codegen perspective
      // because we are binding the argument name to the pointer passed.
      // however, the error could be that we bind that name to the pointer type 
      // when we create the binding within the local scope for this argument.
      // .  
			
			// now we can Codegen the body of the function relative to the local environment
			Outcome<llvm::Value*, Error> body_result = body->Codegen(*local_env);
			
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
       
        llvm::Value* bodyVal = body_result.GetFirst();
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
        // we have to emit a zext instruction
        // and this would be the case for any smaller unsigned integer type, to 
        // any larger integer type (unsigned -> signed OR unsigned -> unsigned).
        // if the smaller integer type is signed and the conversion is
        // signed -> signed then we use the sext instruction.
        // if the smaller integer type is signed and the conversion is 
        // signed -> unsigned then we can also use the zext instruction 
        // and then we have a choice, we could map mathematically to the 
        // unsinged integer and perform an abs instruction to convert 
        // signed to unsigned, then perform the zext OR we could leave the 
        // bits themselves alone when performing the cast, which may be useful 
        // from the perspective of some programmers. if we are doing low level 
        // code, then doing the least implicit bit manipulations will force the 
        // programmer to syntactically perform the bit manipulations
        // themselves. which I think would lead to higher code legibility. 
        //
        // or if we want to convert between our integer type and our boolean
        // type we have to emit a trunc instruction.
        // and this would be the case for any larger integer type to any
        // smaller integer type. (signed -> signed OR unsigned -> unsigned)
        // (the cross sign conversions can either be left alone, or we can
        // consistently perform an abs instruction to map signed values to 
        // unsigned values. in the reverse case we must be aware that we 
        // can destroy data if the unsigned number being represented is a 
        // positive integer above the range of the signed integer.
        //
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
          if (auto error = llvm::InlineAsm::verify(iasm1Ty, "={rdi},i"))
          {
            std::string errstr = "constraint code for iasm1Ty not valid: " + LLVMErrorToString(error);
            FatalError(errstr.data(), __FILE__, __LINE__);
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
          if (auto error = llvm::InlineAsm::verify(iasm1Ty, "={rdi},r"))
          {
            std::string errstr = "constraint code for iasm1Ty not valid" + LLVMErrorToString(error);
            FatalError(errstr.data(), __FILE__, __LINE__);
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


        if (auto error = llvm::InlineAsm::verify(iasm0Ty, "={rax}"))
        {
          std::string errstr = "constraint code for iasm0Ty not valid" + LLVMErrorToString(error);
          FatalError(errstr.data(), __FILE__, __LINE__);
        }

        if (auto error = llvm::InlineAsm::verify(iasm2Ty, ""))
        {
          std::string errstr = "constraint code for iasm2Ty not valid" + LLVMErrorToString(error);
          FatalError(errstr.data(), __FILE__, __LINE__);
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
        Outcome<llvm::Value*, Error> cast_result = Cast(bodyVal, local_builder->getInt64Ty(), *local_env);

        if (!cast_result)
          return cast_result;

        std::vector<llvm::Value*> iasm1Args = { cast_result.GetFirst() };

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
        local_builder->CreateRetVoid();
      }
      else
      {
			  // use the value returned by the body to construct a return statement 
			  // we need to be aware of return value sizes here.
        if (fn_ret_ty->isSingleValueType())
        {
          local_builder->CreateRet(body_result.GetFirst());
        }
        else 
        {
          StoreAggregate(fn_ret_ty, function->getArg(0), body_result.GetFirst(), *local_env);
        }
      }

			// call verifyFunction to validate the generated code for consistency 
			llvm::verifyFunction(*function);
			
      // we now need to fill in the binding of this function with it's
      // definition. so first we erase the incomplete definition, and 
      // then create the full definition.
      env.bindings->Unbind(name);
      env.bindings->Bind(name, getype_result.GetFirst(), function);

			// return the function as the result of code generation
			return Outcome<llvm::Value*, Error>(function);
		}
}


