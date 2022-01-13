#include "type/IntType.hpp"

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

    bool IntType::operator==(Type& other)
    {
        return other.getKind() == Type::Kind::Int;
    }

    std::string IntType::ToString()
    {
        return std::string("Int");
    }
}
