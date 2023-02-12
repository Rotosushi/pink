
#include "runtime/ops/UnopPrimitives.h"

#include "ops/UnopTable.h"

#include "support/DisableWarning.h"

namespace pink {
auto UnopIntNegate(llvm::Value *term, Environment &env) -> llvm::Value * {
  assert(term != nullptr);
  return env.instruction_builder->CreateNeg(term, "neg");
}

auto UnopBoolNegate(llvm::Value *term, Environment &env) -> llvm::Value * {
  assert(term != nullptr);
  return env.instruction_builder->CreateNot(term, "not");
}

auto UnopAddressOfValue(llvm::Value *term, [[maybe_unused]] Environment &env)
    -> llvm::Value * {
  assert(term != nullptr);
  return term;
}

auto UnopValueOfAddress(llvm::Value *term, [[maybe_unused]] Environment &env)
    -> llvm::Value * {
  assert(term != nullptr);
  return term;
}

void InitializeUnopPrimitives(Environment &env) {
  Type::Pointer int_ty      = env.GetIntType();
  Type::Pointer bool_ty     = env.GetBoolType();
  Type::Pointer int_ptr_ty  = env.GetPointerType(int_ty);
  Type::Pointer bool_ptr_ty = env.GetPointerType(bool_ty);

  InternedString neg_str = env.InternOperator("-");
  InternedString not_str = env.InternOperator("!");
  InternedString AOV_str = env.InternOperator("&");
  InternedString VOA_str = env.InternOperator("*");

  env.RegisterUnop(neg_str, int_ty, int_ty, UnopIntNegate);
  env.RegisterUnop(not_str, bool_ty, bool_ty, UnopBoolNegate);
  env.RegisterUnop(AOV_str, int_ty, int_ptr_ty, UnopAddressOfValue);
  env.RegisterUnop(AOV_str, bool_ty, bool_ptr_ty, UnopAddressOfValue);
  env.RegisterUnop(VOA_str, int_ptr_ty, int_ty, UnopValueOfAddress);
  env.RegisterUnop(VOA_str, bool_ptr_ty, bool_ty, UnopValueOfAddress);
}
} // namespace pink
