#pragma once
#include "type/Type.h"

namespace pink {
/**
 * @brief Substitute source_type for target_name within target_type
 *
 * @param type_variable
 * @param source_type
 * @param target_type
 * @return Type::Pointer a Type representing the substituted Type.
 */
auto Substitution(Type::Pointer type_variable,
                  Type::Pointer source_type,
                  Type::Pointer target_type) noexcept -> Type::Pointer;
} // namespace pink
