#include "ast/Binop.h"
#include "aux/Environment.h"

namespace pink {
    Binop::Binop(Location& loc, InternedString o, std::unique_ptr<Ast> l, std::unique_ptr<Ast> r)
        : Ast(Ast::Kind::Binop, loc), op(o), left(std::move(l)), right(std::move(r))
    {

    }

    Binop::~Binop()
    {

    }

    std::unique_ptr<Ast> Binop::Clone()
    {
        return std::make_unique<Binop>(loc, op, left->Clone(), right->Clone());
    }

    bool Binop::classof(const Ast* t)
    {
        return t->getKind() == Ast::Kind::Binop;
    }

    std::string Binop::ToString()
    {
    	std::string result;
    	if (llvm::isa<Binop>(left))
    		result += "(" + left->ToString() + ")";
    	else 
    		result += left->ToString();
    		
    	result += " " + std::string(op) + " ";
    	
    	if (llvm::isa<Binop>(right))
    		result += "(" + right->ToString() + ")";
    	else 
    		result += right->ToString();
    		
        return result;
    }
    
    /*
    	env |- lhs : Tl, rhs : Tr, op : Tl -> Tr -> T
      --------------------------------------------------
      			  env |- lhs op rhs : T
    */
    Outcome<Type*, Error> Binop::GetypeV(Environment& env)
    {
    	// Get the type of both sides
    	Outcome<Type*, Error> lhs_result(left->Getype(env));
    	
    	if (!lhs_result)
    		return lhs_result;
    		
    	Outcome<Type*, Error> rhs_result(right->Getype(env));
    	
    	if (!rhs_result)
    		return rhs_result;
    		
    	// find the operator present between both sides in the env 
    	llvm::Optional<std::pair<InternedString, BinopLiteral*>> binop = env.binops.Lookup(op);
    	
    	if (!binop)
    	{
    		Error error(Error::Kind::Type,
    			std::string("[") + op + std::string("] not bound in env"),
    			loc);
    		Outcome<Type*, Error> result(error);
    		return result;
    	}
    	
    	// find the instance of the operator given the type of both sides
    	llvm::Optional<std::pair<std::pair<Type*, Type*>, BinopCodegen*>> literal = binop->second->Lookup(lhs_result.GetOne(), rhs_result.GetOne());
    	
    	if (!literal)
    	{
    		Error error(Error::Kind::Type,
    			std::string("[") + op + std::string("] has no overload for the given types [")
    				+ lhs_result.GetOne()->ToString() + ", " + rhs_result.GetOne()->ToString() + "]",
    				loc);
    		Outcome<Type*, Error> result(error);
    		return result;
    	}
    	
    	// return the result type of calling the operator given the types of both sides
    	Outcome<Type*, Error> result(literal->second->result_type);
    	return result;
    }
    
    
    Outcome<llvm::Value*, Error> Binop::Codegen(Environment& env)
    {
    // Get the type and value of both sides
    	Outcome<Type*, Error> lhs_type_result(left->Getype(env));
    	
    	if (!lhs_type_result)
    		return Outcome<llvm::Value*, Error>(lhs_type_result.GetTwo());
    		
    	Outcome<llvm::Type*, Error> lhs_type = lhs_type_result.GetOne()->Codegen(env);
    	
    	if (!lhs_type)
    		return Outcome<llvm::Value*, Error>(lhs_type.GetTwo());
    		
    	Outcome<llvm::Value*, Error> lhs_value(left->Codegen(env));
    	
    	if (!lhs_value)
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
    		
    	// find the operator present between both sides in the env 
    	llvm::Optional<std::pair<InternedString, BinopLiteral*>> binop = env.binops.Lookup(op);
    	
    	if (!binop)
    	{
    		Error error(Error::Kind::Semantic,
    			std::string("[") + op + std::string("] not bound in env"),
    			loc);
    		Outcome<llvm::Value*, Error> result(error);
    		return result;
    	}
    	
    	// find the instance of the operator given the type of both sides
    	llvm::Optional<std::pair<std::pair<Type*, Type*>, BinopCodegen*>> literal = binop->second->Lookup(lhs_type_result.GetOne(), rhs_type_result.GetOne());
    	
    	if (!literal)
    	{
    		Error error(Error::Kind::Semantic,
    			std::string("[") + op + std::string("] has no overload for the given types [")
    				+ lhs_type_result.GetOne()->ToString() + ", " + rhs_type_result.GetOne()->ToString() + "]",
    				loc);
    		Outcome<llvm::Value*, Error> result(error);
    		return result;
    	}
    	
    	// use the lhs and rhs values to generate the binop expression.
    	Outcome<llvm::Value*, Error> binop_value(literal->second->generate(lhs_value.GetOne(), rhs_value.GetOne(), env));
    	return binop_value;
    }
}
