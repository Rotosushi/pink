#include "type/BoolType.h"
#include "aux/Environment.h"

namespace pink {
    BoolType::BoolType()
        : Type(Type::Kind::Bool)
    {

    }

    BoolType::~BoolType()
    {

    }

    bool BoolType::classof(const Type* t)
    {
        return t->getKind() == Type::Kind::Bool;
    }

    bool BoolType::EqualTo(Type* other)
    {
        return other->getKind() == Type::Kind::Bool;
    }

    std::string BoolType::ToString()
    {
        return std::string("Bool");
    }
    
    Outcome<llvm::Type*, Error> BoolType::Codegen(const Environment& env)
    {
    	return Outcome<llvm::Type*, Error>(env.instruction_builder->getInt1Ty());
    }
}
