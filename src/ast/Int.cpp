
#include "ast/Int.h"

#include "aux/Environment.h"

namespace pink {
    Int::Int(Location l, int i)
        : Ast(Ast::Kind::Int, l), value(i)
    {

    }

    Int::~Int()
    {

    }

    std::unique_ptr<Ast> Int::Clone()
    {
        return std::make_unique<Int>(loc, value);
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
    Outcome<Type*, Error> Int::GetypeV(Environment& env)
    {
    	Type* int_type = env.types.GetIntType();
    	Outcome<Type*, Error> result(int_type);
    	return result;
    }
    
    /*
       ----------------------
    	env |- [0-9]+ : i64 (value)
    */
    Outcome<llvm::Value*, Error> Int::Codegen(Environment& env)
    {
    	llvm::Value* int_value = env.ir_builder.getInt64(value);
    	Outcome<llvm::Value*, Error> result(int_value);
    	return result;
    }
}
