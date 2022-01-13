#include "type/Type.hpp"

namespace pink {
    Type::Type(Type::Kind k)
        : kind(k)
    {

    }

    Type::~Type()
    {

    }

    Type::Kind Type::getKind() const
    {
        return kind;
    }
}
