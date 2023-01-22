#include "ast/Assignment.h"
#include "aux/Environment.h"

#include "kernel/StoreAggregate.h"

#include "support/LLVMTypeToString.h"
#include "support/LLVMValueToString.h"

namespace pink {
Assignment::Assignment(const Location &location, std::unique_ptr<Ast> left,
                       std::unique_ptr<Ast> right)
    : Ast(Ast::Kind::Assignment, location), left(std::move(left)),
      right(std::move(right)) {}

auto Assignment::classof(const Ast *ast) -> bool {
  return ast->GetKind() == Ast::Kind::Assignment;
}

auto Assignment::ToString() const -> std::string {
  std::string result = left->ToString() + " = " + right->ToString();
  return result;
}

/*
    env |- lhs : Tl, rhs : Tr, Tl == Tr, lhs is-assignable
  ---------------------------------------------------------
                              env |- lhs = rhs : Tl

    note: When we implement local and global variables, the only
            llvm::Value*'s that are going to be bound in the symboltable
            are going to be pointers to places that can be assigned,
            either llvm::AllocaInsts*, or llvm::GlobalVariable*
*/
auto Assignment::TypecheckV(const Environment &env) const
    -> Outcome<Type *, Error> {
  // make sure we can type both sides
  auto lhs_type_result = left->Typecheck(env);
  if (!lhs_type_result) {
    return lhs_type_result;
  }
  auto *lhs_type = lhs_type_result.GetFirst();

  auto rhs_type_result = right->Typecheck(env);
  if (!rhs_type_result) {
    return rhs_type_result;
  }
  auto *rhs_type = rhs_type_result.GetFirst();

  // make sure the left and right hand sides are the same type
  if (lhs_type != rhs_type) {
    std::string errmsg = std::string("storage type: ") + lhs_type->ToString() +
                         ", value type: " + rhs_type->ToString();
    Error error(Error::Code::AssigneeTypeMismatch, GetLoc(), errmsg);
    return {error};
  }
  return {lhs_type};
}

auto Assignment::Codegen(const Environment &env) const
    -> Outcome<llvm::Value *, Error> {
  assert(GetType() != nullptr);
  auto *lhs_type = left->GetType()->ToLLVM(env);

  env.flags->OnTheLHSOfAssignment(true);
  auto lhs_value_result = left->Codegen(env);
  env.flags->OnTheLHSOfAssignment(false);
  if (!lhs_value_result) {
    return lhs_value_result;
  }
  auto *lhs_value = lhs_value_result.GetFirst();

  auto *rhs_type = right->GetType()->ToLLVM(env);

  auto rhs_value_result = right->Codegen(env);
  if (!rhs_value_result) {
    return rhs_value_result;
  }
  auto *rhs_value = rhs_value_result.GetFirst();

  if (lhs_type != rhs_type) {
    std::string errmsg = std::string("storage type: ") +
                         LLVMTypeToString(lhs_type) +
                         ", value type: " + LLVMTypeToString(rhs_type);
    FatalError(errmsg, __FILE__, __LINE__);
  }

  // since even the most basic array type is held within
  // a structure type it suffices to check for that to
  // see if we are storing an aggregate type or not.
  if (llvm::isa<llvm::StructType>(lhs_type)) {
    StoreAggregate(lhs_type, lhs_value, rhs_value, env);
    return {rhs_value};
  }

  if (llvm::isa<llvm::AllocaInst>(lhs_value) ||
      (llvm::isa<llvm::GlobalVariable>(lhs_value)) ||
      (lhs_value->getType()->isPointerTy())) {
    env.instruction_builder->CreateStore(rhs_value, lhs_value);
    return {rhs_value};
  }

  std::string errmsg =
      std::string("llvm::Value* cannot be written: Value* is -> ") +
      LLVMValueToString(lhs_value_result.GetFirst());
  FatalError(errmsg, __FILE__, __LINE__);
  return {Error()};
}
} // namespace pink
