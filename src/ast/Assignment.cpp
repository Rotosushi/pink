#include "ast/Assignment.h"
#include "aux/Environment.h"

namespace pink {
    Assignment::Assignment(Location loc, std::unique_ptr<Ast> left, std::unique_ptr<Ast> right)
        : Ast(Ast::Kind::Assignment, loc), left(std::move(left)), right(std::move(right))
    {

    }

    Assignment::~Assignment()
    {

    }
    
    bool Assignment::classof(const Ast* ast)
    {
    	return ast->getKind() == Ast::Kind::Assignment;
	}

    std::unique_ptr<Ast> Assignment::Clone()
    {
        return std::make_unique<Assignment>(loc, left->Clone(), right->Clone());
    }

    std::string Assignment::ToString()
    {
        std::string result = left->ToString() + " = " + right->ToString();
        return result;
    }
    
    /*
    	env |- lhs : Tl, rhs : Tr, Tl == Tr, lhs is-assignable
      ---------------------------------------------------------
      				  env |- lhs = rhs : Tl 
      	
      	note: When we implement local and global variables, the only 
      		llvm::Value*'s that are going to be bound in the symboltable 
      		are going to be pointers to places that can be assigned,
      		either llvm::AllocaInsts*, or llvm::GlobalVariable*
    */
    Outcome<Type*, Error> Assignment::GetypeV(std::shared_ptr<Environment> env)
    {
    	// make sure we can type both sides
    	Outcome<Type*, Error> lhs_type(left->Getype(env));
    	
    	if (!lhs_type)
    		return lhs_type;
    		
    	Outcome<Type*, Error> rhs_type(right->Getype(env));
    	
    	if (!rhs_type)
    		return rhs_type;
    		
    	// make sure the left and right hand sides are the same type
    	if (lhs_type.GetOne() != rhs_type.GetOne())
    	{
    		Error error(
    			Error::Kind::Type,
    			std::string("[") + lhs_type.GetOne()->ToString() + std::string("] is not equivalent to type [") + rhs_type.GetOne()->ToString() + std::string("]"),
    			loc);
    		return Outcome<Type*, Error>(error);
    	}
    	else 
    	{
    	// the types are equivalent, so it's immaterial which one we return.
    		Outcome<Type*, Error> result(lhs_type.GetOne());
    		return result;
    	}
    }
    
    
    Outcome<llvm::Value*, Error> Assignment::Codegen(std::shared_ptr<Environment> env)
    {
    	// get the type and value of both sides
    	Outcome<Type*, Error> lhs_type_result(left->Getype(env));
    	
    	if (!lhs_type_result)
    		return Outcome<llvm::Value*, Error>(lhs_type_result.GetTwo());
    		
    	Outcome<llvm::Type*, Error> lhs_type = lhs_type_result.GetOne()->Codegen(env);
    	
    	if (!lhs_type)
    		return Outcome<llvm::Value*, Error>(lhs_type.GetTwo());
    		
    	Outcome<llvm::Value*, Error> lhs_value(left->Codegen(env));
    	
    	if(!lhs_value)
    		return lhs_value;
    		
    	Outcome<Type*, Error> rhs_type_result(right->Getype(env));
    	
    	if (!rhs_type_result)
    		return Outcome<llvm::Value*, Error>(rhs_type_result.GetTwo());
    		
    	Outcome<llvm::Type*, Error> rhs_type = rhs_type_result.GetOne()->Codegen(env);
    	
    	if (!rhs_type)
    		return Outcome<llvm::Value*, Error>(rhs_type.GetTwo());
    		
    	Outcome<llvm::Value*, Error> rhs_value(right->Codegen(env));
    	
    	if (!rhs_value)
    		return rhs_value;
    		
    	// make sure the left and right hand sides are the same type
    	if (lhs_type.GetOne() != rhs_type.GetOne())
    	{
    		Error error(
    			Error::Kind::Semantic,
    			std::string("[") + lhs_type_result.GetOne()->ToString() + std::string("] is not equivalent to type [") + rhs_type_result.GetOne()->ToString() + std::string("]"),
    			loc);
    		return Outcome<llvm::Value*, Error>(error);
    	}
    	else 
    	{
    		// we have the correct types, however we cannot assign to a literal value here,
    		// only to a pointer, pointing to a valid memory location in the modules global 
    		// space or the local stack frame, so we check that the bound value 
    		// is able to be assigned to.
    		if (llvm::isa<llvm::AllocaInst>(lhs_value.GetOne()) 
    		|| (llvm::isa<llvm::GlobalVariable>(lhs_value.GetOne())))
    		{
          llvm::Value* right_value = nullptr;
          
          // if the right hand side is a pointer type, then it's a variable, and we have to 
          // construct a load to get the value before we assign.
          if (llvm::isa<llvm::PointerType>(rhs_type.GetOne()))
          {
            right_value = env->builder->CreateLoad(rhs_type.GetOne(), rhs_value.GetOne());
          }
          else // otherwise the value is there directly.
          {
            right_value = rhs_value.GetOne();
          }
          
          env->builder->CreateStore(right_value, lhs_value.GetOne());
          
          // return the value of the right hand side as the result to support nesting assignment
          return Outcome<llvm::Value*, Error>(right_value);
    		}
    		else 
    		{
				  Error error(
						Error::Kind::Semantic,
						std::string("[") + lhs_type_result.GetOne()->ToString() + std::string("] is not an assignable type"), 
						loc);
    			return Outcome<llvm::Value*, Error>(error);
    		}
    	}
    }
}
