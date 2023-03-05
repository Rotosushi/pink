
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
  Type::Pointer integer_type         = env.GetIntType();
  Type::Pointer boolean_type         = env.GetBoolType();
  Type::Pointer integer_pointer_type = env.GetPointerType(integer_type);

  const auto *neg  = env.InternOperator("-");
  const auto *bnot = env.InternOperator("!");
  const auto *aov  = env.InternOperator("&");
  const auto *voa  = env.InternOperator("*");

  env.RegisterUnop(neg, integer_type, integer_type, UnopIntNegate);
  env.RegisterUnop(bnot, boolean_type, boolean_type, UnopBoolNegate);
  env.RegisterUnop(aov, integer_type, integer_pointer_type, UnopAddressOfValue);
  env.RegisterUnop(voa, integer_pointer_type, integer_type, UnopValueOfAddress);
}
} // namespace pink