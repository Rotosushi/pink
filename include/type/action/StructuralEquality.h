#pragma once
#include "type/TypeInterface.h"

namespace pink {
[[nodiscard]] auto StructuralEquality(Type::Pointer one,
                                      Type::Pointer two) noexcept -> bool;
} // namespace pink
