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
    Outcome<Type*, Error> Assignment::Getype(Environment& env)
    {
    	// make sure we can type both sides
    	Outcome<Type*, Error> lhs_result(left->Getype(env));
    	
    	if (!lhs_result)
    		return lhs_result;
    		
    	Outcome<Type*, Error> rhs_result(right->Getype(env));
    	
    	if (!rhs_result)
    		return rhs_result;
    		
    	// make sure the left and right hand sides are the same type
    	if (lhs_result.GetOne() != rhs_result.GetOne())
    	{
    		Error error(
    			Error::Kind::Type,
    			std::string("[") + lhs_result.GetOne()->ToString() + std::string("] is not equivalent to type [") + rhs_result.GetOne()->ToString() + std::string("]"),
    			loc);
    		return Outcome<Type*, Error>(error);
    	}
    	else 
    	{
    		Outcome<Type*, Error> result(lhs_result.GetOne());
    		return result;
    	}
    }
}
