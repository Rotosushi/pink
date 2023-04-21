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
#include "ast/Boolean.h"

#include "aux/Environment.h"

namespace pink {
/* 1/24/2023
  The type of an ast::Boolean is Boolean.

  an ast::Boolean is a temporary, comptime, constant, literal.
*/
auto Boolean::Typecheck(CompilationUnit &unit) const noexcept
    -> Outcome<Type::Pointer, Error> {
  const auto *return_type = unit.GetBoolType();
  SetCachedType(return_type);
  return return_type;
}

/*
  construct a llvm::Constant boolean
*/
auto Boolean::Codegen(CompilationUnit &unit) const noexcept
    -> Outcome<llvm::Value *, Error> {
  return unit.ConstantBoolean(value);
}

void Boolean::Print(std::ostream &stream) const noexcept {
  if (value) {
    stream << "true";
  } else {
    stream << "false";
  }
}
} // namespace pink
