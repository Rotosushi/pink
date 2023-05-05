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
#include "ast/Variable.h"

#include "aux/Environment.h"

namespace pink {
/*
The type of a variable is the type the variable is
bound to in scope if and only if the type is bound
in scope.

the type annotations of a variable are completely dependent upon
the annotations of the type it is bound to.
*/
auto Variable::Typecheck(CompilationUnit &unit) const noexcept
    -> Outcome<Type::Pointer> {
  auto found = unit.LookupVariable(symbol);

  if (found.has_value()) {
    SetCachedType(found->Type());
    return found->Type();
  }

  std::stringstream errmsg;
  errmsg << "[";
  errmsg << symbol;
  errmsg << "]";
  return Error(Error::Code::NameNotBoundInScope,
               GetLocation(),
               std::move(errmsg).str());
}

auto Variable::Codegen(CompilationUnit &unit) const noexcept
    -> Outcome<llvm::Value *> {
  auto bound = unit.LookupVariable(symbol);
  assert(bound.has_value());
  assert(bound->Value() != nullptr);
  return unit.Load(ToLLVM(bound->Type(), unit), bound->Value());
}

void Variable::Print(std::ostream &stream) const noexcept { stream << symbol; }
} // namespace pink
