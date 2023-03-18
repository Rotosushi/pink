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
auto UnopIntNegate(llvm::Value *term, Environment &env) -> llvm::Value * {
  return env.CreateNeg(term);
}

auto UnopBoolNegate(llvm::Value *term, Environment &env) -> llvm::Value * {
  return env.CreateNot(term);
}

/*
 * #REASON: 3/5/2023
 *  AddressOfValue and ValueOfAddress are no-ops
 *  because they are only used to convert types.
 *  llvm always returns pointers to allocations
 *  so we are only choosing whether to load or
 *  not load the pointer, to use in the 'above'
 *  expression
 *
 *  ('above' meaning some parent node 'higher up'
 *   in the AST)
 */
auto UnopAddressOfValue(llvm::Value *term, [[maybe_unused]] Environment &env)
    -> llvm::Value * {
  return term;
}

auto UnopValueOfAddress(llvm::Value *term, [[maybe_unused]] Environment &env)
    -> llvm::Value * {
  return term;
}

void InitializeUnopPrimitives(Environment &env) {
  Type::Pointer integer_type      = env.GetIntType();
  Type::Pointer boolean_type      = env.GetBoolType();
  Type::Pointer type_variable     = env.GetTypeVariable("T");
  Type::Pointer poly_pointer_type = env.GetPointerType(type_variable);

  const auto *neg  = env.InternOperator("-");
  const auto *bnot = env.InternOperator("!");
  const auto *aov  = env.InternOperator("&");
  const auto *voa  = env.InternOperator("*");

  env.RegisterBuiltinUnop(neg, integer_type, integer_type, UnopIntNegate);
  env.RegisterBuiltinUnop(bnot, boolean_type, boolean_type, UnopBoolNegate);

  // Address of Value has type [(T) -> Pointer<T>]
  env.RegisterTemplateBuiltinUnop(aov,
                                  type_variable,
                                  type_variable,
                                  poly_pointer_type,
                                  UnopAddressOfValue);
  // Value of Address has type [(Pointer<T>) -> T]
  env.RegisterTemplateBuiltinUnop(voa,
                                  type_variable,
                                  poly_pointer_type,
                                  type_variable,
                                  UnopValueOfAddress);
}
} // namespace pink