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

#include "aux/Environment.h"

namespace pink {
auto UnopIntNegate(llvm::Value *term, CompilationUnit &env) -> llvm::Value * {
  return env.CreateNeg(term);
}

auto UnopBoolNegate(llvm::Value *term, CompilationUnit &env) -> llvm::Value * {
  return env.CreateNot(term);
}

void InitializeUnopPrimitives(CompilationUnit &env) {
  Type::Pointer integer_type = env.GetIntType();
  Type::Pointer boolean_type = env.GetBoolType();

  env.RegisterUnop(Token::Sub, integer_type, integer_type, UnopIntNegate);
  env.RegisterUnop(Token::Not, boolean_type, boolean_type, UnopBoolNegate);
}
} // namespace pink