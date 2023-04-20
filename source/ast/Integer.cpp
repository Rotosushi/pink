// Copyright (C) 2023 Cade Weinberg
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
#include "ast/Integer.h"

#include "aux/Environment.h"

namespace pink {
auto Integer::Typecheck(CompilationUnit &unit) const noexcept
    -> Outcome<Type::Pointer, Error> {
  auto return_type = unit.GetIntType();
  SetCachedType(return_type);
  return return_type;
}

auto Integer::Codegen(CompilationUnit &unit) const noexcept
    -> Outcome<llvm::Value *, Error> {}

void Integer::Print(std::ostream &stream) const noexcept { stream << value; }
} // namespace pink
