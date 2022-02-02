#include "type/IntType.h"
#include "aux/Environment.h"

namespace pink {
    IntType::IntType()
        : Type(Type::Kind::Int)
    {

    }

    IntType::~IntType()
    {

    }

    bool IntType::classof(const Type* t)
    {
        return t->getKind() == Type::Kind::Int;
    }

    bool IntType::EqualTo(Type* other)
    {
        return other->getKind() == Type::Kind::Int;
    }

    std::string IntType::ToString()
    {
        return std::string("Int");
    }
    
    Outcome<llvm::Type*, Error> IntType::Codegen(Environment& env)
    {
    	return Outcome<llvm::Type*, Error>(env.ir_builder.getInt64Ty());
    }
}
