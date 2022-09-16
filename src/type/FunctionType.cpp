
#include "type/FunctionType.h"

#include "aux/Environment.h"

namespace pink {
		FunctionType::FunctionType(Type* r, std::vector<Type*> a)
			: Type(Type::Kind::Function), result(r), arguments(a)
		{
		
		}
		
		FunctionType::~FunctionType()
		{
		
		}
		
		bool FunctionType::classof(const Type* t)
		{
			return t->getKind() == Type::Kind::Function;
		}
	
		bool FunctionType::EqualTo(Type* other)
		{
			bool equal = true;
		
      // the function types are equal if they have the same 
      // return type, and the same number of argument types,
      // and each argument at each position is identical in type.	
			if (FunctionType* oft = llvm::dyn_cast<FunctionType>(other))
			{
				if (oft->result != result)
				{
					equal = false;
				}
				else 
				{
					int len  = arguments.size();
					int olen = oft->arguments.size();
					
					if (len != olen)
					{
						equal = false;
					}
					else 
					{
						for (int i = 0; i < len; i++)
						{
							if (arguments[i] != oft->arguments[i])
							{
								equal = false;
								break;
							}
						}
						// the types are equal if the evaluation reaches here
					}
				}
			}
			else 
			{
				equal = false;
			}
			
			return equal;
		}
		
		std::string FunctionType::ToString()
		{
			std::string str;
      if (arguments.size() > 0)
      {
		  	for (Type* ty : arguments)
			  {
				  str += ty->ToString() + " -> ";
			  }
      }
      else
      {
        str += "Void -> ";
      }

			str += result->ToString();
			return str;
		}

    /*
     *  So we know that the actual function type that a given function has is 
     *  in some way dependant upon the types within the function type itself.
     *  specifically, any argument which is not a single value type must be 
     *  promoted to a pointer to the same type, and that argument must be given 
     *  the parameter attribute byval(<ty>).
     *  Then, if the return type of the function is similarly not a single
     *  value type, we must set the llvm::FunctionType's return type to void,
     *  and add a parameter with the attribute sret(<ty>).
     *
     *  There is however, a slight complication with this however. namely that 
     *  we cannot add parameter attributes to llvm::FunctionTypes, we instead 
     *  add them to the llvm::Function. So, within this procdure, we simply
     *  need to promote structure type parameters to pointers to the same
     *  structure type, and modify the return type if the return value is a 
     *  structure type too.
     *
     *  I am a little worried that we are destroying information here, because
     *  if we promote the argument to a pointer, than how will
     *  Function::Codegen know to place the byval(<ty>) parameter on that
     *  specific function argument? consider the case where a function takes 
     *  by definition a structure and a pointer to a structure, after
     *  FunctionType::Codegen now both parameters will be pointers to
     *  structures, so how does Function::Codegen know which one is the 
     *  byval(<ty>)? Actually I think we could reference the pink::FunctionType 
     *  to answer that question.
     *
     */    
		Outcome<llvm::Type*, Error> FunctionType::Codegen(const Environment& env)
		{
			std::vector<llvm::Type*> llvm_args;
			
			for (Type* ty : arguments)
			{
				Outcome<llvm::Type*, Error> arg_res = ty->Codegen(env);
				
				if (!arg_res)
				{
					return arg_res;
				}

        // if the llvm::Type is not a single value type,
        // promote the type to a pointer type
        if (arg_res.GetOne()->isSingleValueType())
        {
          llvm_args.emplace_back(arg_res.GetOne());
        }
        else
        {
          // #NOTE: We know that this argument is going to be passed 
          // byvalue to the function, and that llvm is going to
          // perform the copy of the parameters memory. presumably
          // that allocation and subsequent copy must happen in local
          // memory, So i think the address space for this pointer 
          // is the Alloca address space.
          llvm_args.emplace_back(env.instruction_builder->getPtrTy(env.data_layout.getAllocaAddrSpace()));
        }
			}
			
			Outcome<llvm::Type*, Error> res_res = result->Codegen(env);
			
			if (!res_res)
			{
				return res_res;
			}
			
			if (llvm_args.size() > 0)
			{ 
        if (res_res.GetOne()->isSingleValueType() || res_res.GetOne()->isVoidTy())
        {
				  llvm::Type* fn_ty = llvm::FunctionType::get(res_res.GetOne(), llvm_args, /* isVarArg */ false);
				  return Outcome<llvm::Type*, Error>(fn_ty);
        }
        else
        {
          // promote return value to an argument as a pointer
          llvm_args.insert(llvm_args.begin(), env.instruction_builder->getPtrTy(env.data_layout.getAllocaAddrSpace()));
          llvm::Type* fn_ty = llvm::FunctionType::get(env.instruction_builder->getVoidTy(), llvm_args, /* isVararg */ false);
          return Outcome<llvm::Type*, Error>(fn_ty);
        }
			}
			else 
			{
        llvm::Type* res_ty = res_res.GetOne();
        if (res_ty->isSingleValueType() || res_ty->isVoidTy())
        {
				  llvm::Type* fn_ty = llvm::FunctionType::get(res_res.GetOne(), /* isVararg */ false);
				  return Outcome<llvm::Type*, Error>(fn_ty);
			  }
        else
        {
          // promote the return value to an argument as a pointer
          llvm_args.insert(llvm_args.begin(), env.instruction_builder->getPtrTy(env.data_layout.getAllocaAddrSpace()));
          llvm::Type* fn_ty = llvm::FunctionType::get(env.instruction_builder->getVoidTy(), llvm_args, /* isVarArg */ false);
          return Outcome<llvm::Type*, Error>(fn_ty);
        }
      }
		}
}
