#include "type/NilType.hpp"


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
        return t->GetKind() == Type::Kind::Nil;
    }

    bool NilType::operator==(Type& other)
    {
        return other.GetKind() == Type::Kind::Nil;
    }

    std::string NilType::ToString()
    {
        return std::string("Nil");
    }
}
