#include "type/BoolType.hpp"

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
        return t->GetKind() == Type::Kind::Bool;
    }

    bool BoolType::operator==(Type& other)
    {
        return other.GetKind() == Type::Kind::Bool;
    }

    std::string BoolType::ToString()
    {
        return std::string("Bool");
    }
}
