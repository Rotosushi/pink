#include "ast/Unop.h"
#include "aux/Environment.h"

namespace pink {
    Unop::Unop(Location& loc, InternedString o, Ast* r)
        : Ast(Ast::Kind::Unop, loc), op(o), right(r)
    {

    }

    Unop::~Unop()
    {

    }

    Ast* Unop::Clone()
    {
        return new Unop(loc, op, right->Clone());
    }

    bool Unop::classof(const Ast* t)
    {
        return t->getKind() == Ast::Kind::Unop;
    }

    std::string Unop::ToString()
    {
        return std::string(op) + right->ToString();
    }
    
    /*
    	    env |- rhs : Tr, op : Tr -> T
      ----------------------------------------
      			env |- op rhs : T
    */
    Outcome<Type*, Error> Unop::Getype(Environment& env)
    {
    	// get the type of the rhs
    	Outcome<Type*, Error> rhs_result(right->Getype(env));
    	
    	if (!rhs_result)
    		return rhs_result;
    	
    	// find the operator used in the env
    	llvm::Optional<std::pair<InternedString, UnopLiteral*>> unop = env.unops.Lookup(op);
    	
    	if (!unop)
    	{
    		Outcome<Type*, Error> result(Error(Error::Kind::Type,
    			std::string("[") + op + std::string("] not bound in env"), 
    			loc));
    		return result;
    	}
    	
    	// find the overload of the operator for the given type
    	llvm::Optional<std::pair<Type*, UnopCodegen*>> literal = unop->second->Lookup(rhs_result.GetOne());
    	
    	if (!literal)
    	{
    		Outcome<Type*, Error> result(Error(Error::Kind::Type,
    			std::string("[") + op + std::string("] has no overload for given type [")
    				+ rhs_result.GetOne()->ToString() + "]",
    			loc));
    		return result;
    	}
    	
    	// return the result type of applying the operator to the given type
    	Outcome<Type*, Error> result(literal->second->result_type);
    	return result;
    }
}
