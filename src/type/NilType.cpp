#include "type/NilType.h"
#include "aux/Environment.h"

namespace pink {
    NilType::NilType()
        : Type(Type::Kind::Nil)
    {

    }

    NilType::~NilType()
    {

    }

    bool NilType::classof(const Type* t)
    {
        return t->getKind() == Type::Kind::Nil;
    }

    bool NilType::operator==(Type& other)
    {
        return other.getKind() == Type::Kind::Nil;
    }

    std::string NilType::ToString()
    {
        return std::string("Nil");
    }
    
    Outcome<llvm::Type*, Error> NilType::Codegen(Environment& env)
    {
    	return Outcome<llvm::Type*, Error>(env.ir_builder.getInt1Ty());
    }
}
