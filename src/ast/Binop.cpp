#include "ast/Binop.h"


namespace pink {
    Binop::Binop(Location& loc, InternedString o, Ast* l, Ast* r)
        : Ast(Ast::Kind::Binop, loc), op(o), left(l), right(r)
    {

    }

    Binop::~Binop()
    {
        delete left;
        delete right;
    }

    Ast* Binop::Clone()
    {
        return new Binop(loc, op, left->Clone(), right->Clone());
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
    Outcome<Type*, Error> Binop::Getype(Environment& env)
    {
    
    }
}
