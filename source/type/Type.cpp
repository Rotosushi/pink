#include "type/Type.h"

namespace pink {
Type::Type(Type::Kind kind) : kind(kind) {}

auto Type::GetKind() const -> Type::Kind { return kind; }
} // namespace pink
