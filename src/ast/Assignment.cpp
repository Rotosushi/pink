#include "ast/Assignment.h"

namespace pink {
    Assignment::Assignment(Location loc, Ast* left, Ast* right)
        : Ast(Ast::Kind::Assignment, loc), left(left), right(right)
    {

    }

    Assignment::~Assignment()
    {
        delete left;
        delete right;
    }
    
    bool Assignment::classof(const Ast* ast)
    {
    	return ast->getKind() == Ast::Kind::Assignment;
	}

    Ast* Assignment::Clone()
    {
        return new Assignment(loc, left->Clone(), right->Clone());
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
    */
    Outcome<Type*, Error> Assignment::Getype(Environment& env)
    {
    
    }
}
