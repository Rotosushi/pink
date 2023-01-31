#pragma once
#include "type/Type.h"

namespace pink {

[[nodiscard]] auto StructuralEquality(Type::Pointer one,
                                      Type::Pointer two) noexcept -> bool;
} // namespace pink
