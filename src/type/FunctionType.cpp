
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
			for (Type* ty : arguments)
			{
				str += ty->ToString() + " -> ";
			}
			
			str += result->ToString();
			return str;
		}
		
		Outcome<llvm::Type*, Error> FunctionType::Codegen(Environment& env)
		{
			std::vector<llvm::Type*> llvm_args;
			
			for (Type* ty : arguments)
			{
				Outcome<llvm::Type*, Error> arg_res = ty->Codegen(env);
				
				if (!arg_res)
				{
					return arg_res;
				}
				
				llvm_args.emplace_back(arg_res.GetOne());
			}
			
			Outcome<llvm::Type*, Error> res_res = result->Codegen(env);
			
			if (!res_res)
			{
				return res_res;
			}
			
			if (llvm_args.size() > 0)
			{
				llvm::Type* fn_ty = llvm::FunctionType::get(res_res.GetOne(), llvm_args, /* isVarArg */ false);
				return Outcome<llvm::Type*, Error>(fn_ty);
			}
			else 
			{
				llvm::Type* fn_ty = llvm::FunctionType::get(res_res.GetOne(), /* isVararg */ false);
				return Outcome<llvm::Type*, Error>(fn_ty);
			}
		}
}
