#pragma once
#include "type/TypeInterface.h"

namespace pink {
[[nodiscard]] auto StructuralEquality(TypeInterface::Pointer one,
                                      TypeInterface::Pointer two) noexcept
    -> bool;
} // namespace pink
