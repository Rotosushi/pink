#include "ast/Bool.h"

#include "aux/Environment.h"

namespace pink {
    Bool::Bool(Location l, bool b)
        : Ast(Ast::Kind::Bool, l), value(b)
    {

    }

    Bool::~Bool()
    {

    }

    bool Bool::classof(const Ast* a)
    {
        return a->getKind() == Ast::Kind::Bool;
    }

    std::string Bool::ToString()
    {
        if (value)
            return std::string("true");
        else
            return std::string("false");
    }
    
    /*
       ---------------------
    	env |- true : Bool 
    	
       ---------------------
    	env |- false : Bool
    */
    Outcome<Type*, Error> Bool::GetypeV(const Environment& env)
    {
    	Type* bool_type = env.types->GetBoolType();
    	Outcome<Type*, Error> result(bool_type);
    	return result;
    }
    
    /*
    	---------------------
    	env |- true : i1 (1) 
    	
       ---------------------
    	env |- false : i1 (0)
    */
    Outcome<llvm::Value*, Error> Bool::Codegen(const Environment& env)
    {
    	llvm::Value* bool_value = env.instruction_builder->getInt1(value);
    	Outcome<llvm::Value*, Error> result(bool_value);
    	return result;
    }
}
