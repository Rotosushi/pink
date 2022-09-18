
#include "ast/Int.h"

#include "aux/Environment.h"

namespace pink {
    Int::Int(Location l, long long i)
        : Ast(Ast::Kind::Int, l), value(i)
    {

    }

    Int::~Int()
    {

    }

    bool Int::classof(const Ast* a)
    {
        return a->getKind() == Ast::Kind::Int;
    }

    std::string Int::ToString()
    {
        return std::to_string(value);
    }
    
    /*
       ----------------------
    	env |- [0-9]+ : Int
    */
    Outcome<Type*, Error> Int::GetypeV(const Environment& env)
    {
    	Type* int_type = env.types->GetIntType();
    	Outcome<Type*, Error> result(int_type);
    	return result;
    }
    
    /*
       ----------------------
    	env |- [0-9]+ : i64 (value)
    */
    Outcome<llvm::Value*, Error> Int::Codegen(const Environment& env)
    {
    	llvm::Value* int_value = env.instruction_builder->getInt64(value);
    	Outcome<llvm::Value*, Error> result(int_value);
    	return result;
    }
}
