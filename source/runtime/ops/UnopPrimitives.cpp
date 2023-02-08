
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

auto UnopAddressOfValue(llvm::Value *term, Environment &env) -> llvm::Value * {
  assert(term != nullptr);
  return term;
}

auto UnopValueOfAddress(llvm::Value *term, Environment &env) -> llvm::Value * {
  assert(term != nullptr);
  return term;
}

void InitializeUnopPrimitives(Environment &env) {
  Type::Pointer int_ty      = env.type_interner.GetIntType();
  Type::Pointer bool_ty     = env.type_interner.GetBoolType();
  Type::Pointer int_ptr_ty  = env.type_interner.GetPointerType(int_ty);
  Type::Pointer bool_ptr_ty = env.type_interner.GetPointerType(bool_ty);

  InternedString neg_str = env.operator_interner.Intern("-");
  InternedString not_str = env.operator_interner.Intern("!");
  InternedString AOV_str = env.operator_interner.Intern("&");
  InternedString VOA_str = env.operator_interner.Intern("*");

  env.unop_table.Register(neg_str, int_ty, int_ty, UnopIntNegate);
  env.unop_table.Register(not_str, bool_ty, bool_ty, UnopBoolNegate);

  env.unop_table.Register(AOV_str, int_ty, int_ptr_ty, UnopAddressOfValue);
  env.unop_table.Register(AOV_str, bool_ty, bool_ptr_ty, UnopAddressOfValue);

  env.unop_table.Register(VOA_str, int_ptr_ty, int_ty, UnopValueOfAddress);
  env.unop_table.Register(VOA_str, bool_ptr_ty, bool_ty, UnopValueOfAddress);
}
} // namespace pink
