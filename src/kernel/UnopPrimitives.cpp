
#include "kernel/UnopPrimitives.h"

#include "ops/UnopTable.h"

#include "support/DisableWarning.h"

namespace pink {
auto UnopIntNegate(llvm::Value *term, const Environment &env)
    -> Outcome<llvm::Value *, Error> {
  assert(term != nullptr);
  return {env.instruction_builder->CreateNeg(term, "neg")};
}

auto UnopBoolNegate(llvm::Value *term, const Environment &env)
    -> Outcome<llvm::Value *, Error> {
  assert(term != nullptr);
  return {env.instruction_builder->CreateNot(term, "not")};
}

/*
  UnopAddressOfValue and UnopValueOfAddress make sense on all possible types,
  because any possible instance of any possible type must live at some memory
  location so it always makes sense to construct a pointer to any possible type.

  thus we do not need to do anything to support the semantics of either
  operation. as it all truly depends on how we treat the pointer held in any
  given llvm::Value*

  given this, we do not make use of an argument which is necessary for each
  other unop I can imagine. and more pertinently all other unops we have
  defined.

  then since any function which implements a unop must have a function type
  matching the function pointer type to be stored in the Look up table we have
  constructed, this function unfortunately has to have an extra parameter.

  so, we are going to ignore the normal warning of an unused parameter.
  such that the code compiles with zero warnings.

*/

// note: turn off formatting here because clag-format interprets
// -Wunused-parameter as -Wunused - parameter which breaks
// this macro. also turn off linting, because we know the env
// parameter is unused. It must be there for this function to
// fit the type of a UnopCodegenFn function pointer.
// clang-format off
// NOLINTBEGIN
NOWARN(-Wunused-parameter,
auto UnopAddressOfValue(llvm::Value *term, const Environment &env)
  -> Outcome<llvm::Value *, Error> {
  return Outcome<llvm::Value *, Error>(term);
}

auto UnopValueOfAddress(llvm::Value *term, const Environment &env)
  -> Outcome<llvm::Value *, Error> {
  return Outcome<llvm::Value *, Error>(term);
})
// NOLINTEND
// clang-format on

void InitializeUnopPrimitives(const Environment &env) {
  Type *int_ty = env.types->GetIntType();
  Type *bool_ty = env.types->GetBoolType();
  Type *int_ptr_ty = env.types->GetPointerType(int_ty);
  Type *bool_ptr_ty = env.types->GetPointerType(bool_ty);

  InternedString neg_str = env.operators->Intern("-");
  InternedString not_str = env.operators->Intern("!");
  InternedString AOV_str = env.operators->Intern("&");
  InternedString VOA_str = env.operators->Intern("*");

  env.unops->Register(neg_str, int_ty, int_ty, UnopIntNegate);
  env.unops->Register(not_str, bool_ty, bool_ty, UnopBoolNegate);

  env.unops->Register(AOV_str, int_ty, int_ptr_ty, UnopAddressOfValue);
  env.unops->Register(AOV_str, bool_ty, bool_ptr_ty, UnopAddressOfValue);

  env.unops->Register(VOA_str, int_ptr_ty, int_ty, UnopValueOfAddress);
  env.unops->Register(VOA_str, bool_ptr_ty, bool_ty, UnopValueOfAddress);
}
} // namespace pink
