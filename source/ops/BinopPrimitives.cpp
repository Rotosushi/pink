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
auto BinopIntAdd(llvm::Value *left, llvm::Value *right, CompilationUnit &env)
    -> llvm::Value * {
  return env.CreateAdd(left, right, "iadd");
}

auto BinopIntSub(llvm::Value *left, llvm::Value *right, CompilationUnit &env)
    -> llvm::Value * {
  return env.CreateSub(left, right, "isub");
}

auto BinopIntMul(llvm::Value *left, llvm::Value *right, CompilationUnit &env)
    -> llvm::Value * {
  return env.CreateMul(left, right, "imul");
}

auto BinopIntSDiv(llvm::Value *left, llvm::Value *right, CompilationUnit &env)
    -> llvm::Value * {
  return env.CreateSDiv(left, right, "idiv");
}

auto BinopIntMod(llvm::Value *left, llvm::Value *right, CompilationUnit &env)
    -> llvm::Value * {
  return env.CreateSRem(left, right, "imod");
}

auto BinopIntEq(llvm::Value *left, llvm::Value *right, CompilationUnit &env)
    -> llvm::Value * {
  return env.CreateICmpEQ(left, right, "ieq");
}

auto BinopBoolEq(llvm::Value *left, llvm::Value *right, CompilationUnit &env)
    -> llvm::Value * {
  return env.CreateICmpEQ(left, right, "ieq");
}

auto BinopIntNe(llvm::Value *left, llvm::Value *right, CompilationUnit &env)
    -> llvm::Value * {
  return env.CreateICmpNE(left, right, "ineq");
}

auto BinopBoolNe(llvm::Value *left, llvm::Value *right, CompilationUnit &env)
    -> llvm::Value * {
  return env.CreateICmpNE(left, right, "boolneq");
}

auto BinopIntGt(llvm::Value *left, llvm::Value *right, CompilationUnit &env)
    -> llvm::Value * {
  return env.CreateICmpSGT(left, right, "igt");
}

auto BinopIntGe(llvm::Value *left, llvm::Value *right, CompilationUnit &env)
    -> llvm::Value * {
  return env.CreateICmpSGE(left, right, "ige");
}

auto BinopIntLt(llvm::Value *left, llvm::Value *right, CompilationUnit &env)
    -> llvm::Value * {
  return env.CreateICmpSLT(left, right, "ilt");
}

auto BinopIntLe(llvm::Value *left, llvm::Value *right, CompilationUnit &env)
    -> llvm::Value * {
  return env.CreateICmpSLE(left, right, "ile");
}

auto BinopBoolAnd(llvm::Value *left, llvm::Value *right, CompilationUnit &env)
    -> llvm::Value * {
  return env.CreateAnd(left, right, "and");
}

auto BinopBoolOr(llvm::Value *left, llvm::Value *right, CompilationUnit &env)
    -> llvm::Value * {
  return env.CreateOr(left, right, "or");
}

void InitializeBinopPrimitives(CompilationUnit &env) {
  Type::Pointer int_ty  = env.GetIntType();
  Type::Pointer bool_ty = env.GetBoolType();

  env.RegisterBinop(Token::Equals, int_ty, int_ty, bool_ty, BinopIntEq);
  env.RegisterBinop(Token::Equals, bool_ty, bool_ty, bool_ty, BinopBoolEq);
  env.RegisterBinop(Token::NotEquals, int_ty, int_ty, bool_ty, BinopIntNe);
  env.RegisterBinop(Token::NotEquals, bool_ty, bool_ty, bool_ty, BinopBoolNe);
  env.RegisterBinop(Token::LessThan, int_ty, int_ty, bool_ty, BinopIntLt);
  env.RegisterBinop(Token::LessThanOrEqual,
                    int_ty,
                    int_ty,
                    bool_ty,
                    BinopIntLe);
  env.RegisterBinop(Token::GreaterThan, int_ty, int_ty, bool_ty, BinopIntGt);
  env.RegisterBinop(Token::GreaterThanOrEqual,
                    int_ty,
                    int_ty,
                    bool_ty,
                    BinopIntGe);
  env.RegisterBinop(Token::And, bool_ty, bool_ty, bool_ty, BinopBoolAnd);
  env.RegisterBinop(Token::Or, bool_ty, bool_ty, bool_ty, BinopBoolOr);
  env.RegisterBinop(Token::Add, int_ty, int_ty, int_ty, BinopIntAdd);
  env.RegisterBinop(Token::Sub, int_ty, int_ty, int_ty, BinopIntSub);
  env.RegisterBinop(Token::Star, int_ty, int_ty, int_ty, BinopIntMul);
  env.RegisterBinop(Token::Divide, int_ty, int_ty, int_ty, BinopIntSDiv);
  env.RegisterBinop(Token::Modulo, int_ty, int_ty, int_ty, BinopIntMod);
}
} // namespace pink
