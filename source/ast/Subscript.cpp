#include "ast/Subscript.h"

#include "visitor/AstVisitor.h"

#include "kernel/ArraySubscript.h"
#include "kernel/SliceSubscript.h"

namespace pink {
Subscript::Subscript(const Location &location, std::unique_ptr<Ast> left,
                     std::unique_ptr<Ast> right)
    : Ast(Ast::Kind::Subscript, location), left(std::move(left)),
      right(std::move(right)) {}

void Subscript::Accept(AstVisitor *visitor) const {
  return visitor->Visit(this);
}

auto Subscript::classof(const Ast *ast) -> bool {
  return ast->GetKind() == Ast::Kind::Subscript;
}

auto Subscript::ToString() const -> std::string {
  return left->ToString() + "[" + right->ToString() + "]";
}

/*
  lhs has to be an Array or a Slice
  rhs has to be an Int (or an Int like)
*/
auto Subscript::TypecheckV(const Environment &env) const
    -> Outcome<Type *, Error> {
  auto rhs_type_result = right->Typecheck(env);
  if (!rhs_type_result) {
    return rhs_type_result;
  }
  Type *rhs_type = rhs_type_result.GetFirst();

  if ((llvm::dyn_cast<IntType>(rhs_type)) == nullptr) {
    std::string errmsg =
        "cannot use type: " + rhs_type->ToString() + " as an index";
    return {Error(Error::Code::SubscriptRightIsNotAnIndex, GetLoc(), errmsg)};
  }

  auto lhs_type_result = left->Typecheck(env);
  if (!lhs_type_result) {
    return lhs_type_result;
  }
  Type *lhs_type = lhs_type_result.GetFirst();

  Type *element_type = nullptr;
  if (auto *array_type = llvm::dyn_cast<ArrayType>(lhs_type);
      array_type != nullptr) {
    element_type = array_type->member_type;
  } else if (auto *slice_type = llvm::dyn_cast<SliceType>(lhs_type);
             slice_type != nullptr) {
    element_type = slice_type->pointee_type;
  } else {
    std::string errmsg =
        "cannot subscript object of type: " + lhs_type->ToString();
    return {
        Error(Error::Code::SubscriptLeftIsNotSubscriptable, GetLoc(), errmsg)};
  }
  assert(element_type != nullptr);
  return {element_type};
}

auto Subscript::Codegen(const Environment &env) const
    -> Outcome<llvm::Value *, Error> {
  Type *lhs_type = left->GetType();
  assert(lhs_type != nullptr);
  Type *rhs_type = right->GetType();
  assert(rhs_type != nullptr);

  auto lhs_codegen_result = left->Codegen(env);
  if (!lhs_codegen_result) {
    return lhs_codegen_result;
  }
  auto *lhs_value = lhs_codegen_result.GetFirst();
  assert(lhs_value != nullptr);

  pink::Outcome<llvm::Value *, pink::Error> rhs_codegen_result = Error();
  llvm::Value *rhs_value = nullptr;
  if (env.flags->OnTheLHSOfAssignment()) {
    // pretend we are not on the lhs of assignment
    // in order to load variables as indicies
    env.flags->OnTheLHSOfAssignment(false);
    rhs_codegen_result = right->Codegen(env);
    if (!rhs_codegen_result) {
      return rhs_codegen_result;
    }
    rhs_value = rhs_codegen_result.GetFirst();
    assert(rhs_value != nullptr);
    env.flags->OnTheLHSOfAssignment(true);
  } else {
    rhs_codegen_result = right->Codegen(env);
    if (!rhs_codegen_result) {
      return rhs_codegen_result;
    }
    rhs_value = rhs_codegen_result.GetFirst();
    assert(rhs_value != nullptr);
  }

  if (auto *array_type = llvm::dyn_cast<ArrayType>(lhs_type);
      array_type != nullptr) {
    auto *llvm_array_type =
        llvm::cast<llvm::StructType>(array_type->ToLLVM(env));
    auto *llvm_element_type = array_type->member_type->ToLLVM(env);
    return {ArraySubscript(llvm_array_type, llvm_element_type, lhs_value,
                           rhs_value, env)};
  }

  if (auto *slice_type = llvm::dyn_cast<SliceType>(lhs_type);
      slice_type != nullptr) {
    auto *llvm_slice_type =
        llvm::cast<llvm::StructType>(slice_type->ToLLVM(env));
    auto *element_type = slice_type->pointee_type->ToLLVM(env);
    return {SliceSubscript(llvm_slice_type, element_type, lhs_value, rhs_value,
                           env)};
  }

  std::string errmsg =
      "cannot subscript object of type: " + lhs_type->ToString();
  FatalError(errmsg, __FILE__, __LINE__);
  return {Error()};
}

} // namespace pink