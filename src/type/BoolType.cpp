#include "type/BoolType.h"

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

    bool BoolType::operator==(Type& other)
    {
        return other.getKind() == Type::Kind::Bool;
    }

    std::string BoolType::ToString()
    {
        return std::string("Bool");
    }
}
