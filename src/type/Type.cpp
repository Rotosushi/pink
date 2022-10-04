#include "type/Type.h"

namespace pink {
Type::Type(Type::Kind kind) : kind(kind) {}

auto Type::getKind() const -> Type::Kind { return kind; }
} // namespace pink
