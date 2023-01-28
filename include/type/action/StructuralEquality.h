#pragma once
#include "type/Type.h"

namespace pink {

[[nodiscard]] auto StructuralEquality(const Type::Pointer one,
                                      const Type::Pointer two) noexcept -> bool;
} // namespace pink
