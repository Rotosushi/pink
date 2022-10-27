#include "ast/Assignment.h"
#include "aux/Environment.h"

#include "kernel/StoreAggregate.h"

#include "support/LLVMTypeToString.h"
#include "support/LLVMValueToString.h"

#include "visitor/AstVisitor.h"

namespace pink {
Assignment::Assignment(const Location &location, std::unique_ptr<Ast> left,
                       std::unique_ptr<Ast> right)
    : Ast(Ast::Kind::Assignment, location), left(std::move(left)),
      right(std::move(right)) {}

void Assignment::Accept(AstVisitor *visitor) const { visitor->Visit(this); }

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
  Outcome<Type *, Error> lhs_type(left->Typecheck(env));

  if (!lhs_type) {
    return lhs_type;
  }

  Outcome<Type *, Error> rhs_type(right->Typecheck(env));

  if (!rhs_type) {
    return rhs_type;
  }

  // make sure the left and right hand sides are the same type
  if (lhs_type.GetFirst() != rhs_type.GetFirst()) {
    std::string errmsg = std::string("storage type: ") +
                         lhs_type.GetFirst()->ToString() +
                         ", value type: " + rhs_type.GetFirst()->ToString();
    Error error(Error::Code::AssigneeTypeMismatch, GetLoc(), errmsg);
    return {error};
  }
  // the types are equivalent, so it's immaterial which one we return.
  return {lhs_type.GetFirst()};
}

auto Assignment::Codegen(const Environment &env) const
    -> Outcome<llvm::Value *, Error> {
  assert(GetType() != nullptr);
  llvm::Type *lhs_type = left->GetType()->ToLLVM(env);

  env.flags->OnTheLHSOfAssignment(true);
  auto lhs_value = left->Codegen(env);
  env.flags->OnTheLHSOfAssignment(false);
  if (!lhs_value) {
    return lhs_value;
  }

  llvm::Type *rhs_type = right->GetType()->ToLLVM(env);

  auto rhs_value = right->Codegen(env);
  if (!rhs_value) {
    return rhs_value;
  }

  if (lhs_type != rhs_type) {
    std::string errmsg = std::string("storage type: ") +
                         LLVMTypeToString(lhs_type) +
                         ", value type: " + LLVMTypeToString(rhs_type);
    FatalError(errmsg, __FILE__, __LINE__);
  }

  if (llvm::isa<llvm::StructType>(lhs_type)) {
    llvm::Value *right_value = rhs_value.GetFirst();

    StoreAggregate(lhs_type, lhs_value.GetFirst(), rhs_value.GetFirst(), env);

    return {right_value};
  }

  if (llvm::isa<llvm::AllocaInst>(lhs_value.GetFirst()) ||
      (llvm::isa<llvm::GlobalVariable>(lhs_value.GetFirst())) ||
      (lhs_value.GetFirst()->getType()->isPointerTy())) {
    llvm::Value *right_value = rhs_value.GetFirst();

    env.instruction_builder->CreateStore(right_value, lhs_value.GetFirst());

    // return the value of the right hand side as the result to support nesting
    // assignment
    return {right_value};
  }

  std::string errmsg =
      std::string("llvm::Value* cannot be written: Value* is -> ") +
      LLVMValueToString(lhs_value.GetFirst());
  Error error(Error::Code::ValueCannotBeAssigned, GetLoc(), errmsg);
  return {error};
}
} // namespace pink
