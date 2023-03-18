// Copyright (C) 2023 cadence
// 
// This file is part of pink.
// 
// pink is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// pink is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with pink.  If not, see <http://www.gnu.org/licenses/>.

#pragma once
#include <memory> // std::unique_ptr
#include <string> // std::string

#include "ast/Ast.h"

#include "ast/visitor/AstVisitor.h"
#include "visitor/VisitorResult.h"

namespace pink {

/**
 * @brief Computes the canonical string representation of the Ast
 *
 * by canonical I simply mean that if the [parser](#Parser) were to read in
 * the string returned by ToString it would construct the exact same Ast as
 * generated the string. That is, Parser::Parse() and ToString() are
 * inverse operations. (modulo textual formatting)
 *
 * \todo ToString ends up doing a lot of intermediate allocations
 * and concatenations as it builds up it's result, this seems like
 * a good place to consider optimizing. one possible speedup might
 * be to buffer all of the strings into a list, and then perform
 * a single concatenation at the end. This might speed up the algorithm
 * overall. as there may be less total allocations, and there will be
 * no repeated copying of the intermediate strings.
 *
 * \todo This function does not currently indent blocks of code
 * according to their nested depth. and I feel that it should.
 * maybe we should write a formatter. (once we have the syntax
 * nailed down)
 *
 * @return std::string the canonical string representing this Ast
 */
[[nodiscard]] auto ToString(const Ast::Pointer &ast) noexcept -> std::string;
} // namespace pink
