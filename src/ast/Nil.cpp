#include "ast/Nil.h"

#include "aux/Environment.h"

namespace pink {
    Nil::Nil(Location l)
        : Ast(Ast::Kind::Nil, l)
    {

    }

    Nil::~Nil()
    {

    }

    std::unique_ptr<Ast> Nil::Clone()
    {
        return std::make_unique<Nil>(loc);
    }

    bool Nil::classof(const Ast* a)
    {
        return a->getKind() == Ast::Kind::Nil;
    }

    std::string Nil::ToString()
    {
        return std::string("nil");
    }
    
    /*
    	--------------------
    	  env |- nil : Nil
    */
    Outcome<Type*, Error> Nil::GetypeV(Environment& env)
    {
    	Type* nil_type = env.types.GetNilType();
    	Outcome<Type*, Error> result(nil_type);
    	return result;
    }
    
    /*
    	---------------------
    	env |- nil : i1 (0)
    */
    Outcome<llvm::Value*, Error> Nil::Codegen(Environment& env)
    {
    	llvm::Value* nil_value = env.ir_builder.getFalse();
    	Outcome<llvm::Value*, Error> result(nil_value);
    	return result;
    }
}
