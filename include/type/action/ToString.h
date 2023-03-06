#pragma once
#include "type/visitor/TypeVisitor.h"
#include "visitor/VisitorResult.h"

#include "type/TypeInterface.h"

namespace pink {
/**
 * @brief Computes the string representation of the given Type
 *
 * @param type
 * @return std::string
 */
[[nodiscard]] auto ToString(TypeInterface::Pointer type) noexcept
    -> std::string;
} // namespace pink
