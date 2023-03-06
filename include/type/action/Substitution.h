#pragma once
#include "type/TypeInterface.h"

namespace pink {
/**
 * @brief Substitute source_type for target_name within target_type
 *
 * @param type_variable
 * @param source_type
 * @param target_type
 * @return Type::Pointer a Type representing the substituted Type.
 */
auto Substitution(TypeInterface::Pointer type_variable,
                  TypeInterface::Pointer source_type,
                  TypeInterface::Pointer target_type) noexcept
    -> TypeInterface::Pointer;
} // namespace pink
