
#include "ast/Conditional.h"

#include "aux/Environment.h"

#include "visitor/AstVisitor.h"

namespace pink {
Conditional::Conditional(const Location &location, std::unique_ptr<Ast> test,
                         std::unique_ptr<Ast> first,
                         std::unique_ptr<Ast> second)
    : Ast(Ast::Kind::Conditional, location), test(std::move(test)),
      first(std::move(first)), second(std::move(second)) {}

void Conditional::Accept(AstVisitor *visitor) const { visitor->Visit(this); }

auto Conditional::classof(const Ast *ast) -> bool {
  return ast->GetKind() == Ast::Kind::Conditional;
}

auto Conditional::ToString() const -> std::string {
  std::string result = "if ";
  result += test->ToString();
  result += " then ";
  result += first->ToString();
  result += " else ";
  result += second->ToString();
  return result;
}

/*
 *  The type of a conditional is based on two checks,
 *  1) the type of the test expression is Bool.
 *  2) the type of both alternative expressions must match.
 *
 */
auto Conditional::TypecheckV(const Environment &env) const
    -> Outcome<Type *, Error> {
  auto test_type_result = test->Typecheck(env);
  if (!test_type_result) {
    return test_type_result;
  }
  auto *test_type = test_type_result.GetFirst();

  Type *bool_ty = env.types->GetBoolType();
  if (bool_ty != test_type) {
    std::string errmsg =
        std::string("test expression has type: ") + test_type->ToString();
    return {
        Error(Error::Code::CondTestExprTypeMismatch, test->GetLoc(), errmsg)};
  }

  auto first_type_result = first->Typecheck(env);
  if (!first_type_result) {
    return first_type_result;
  }
  auto *first_type = first_type_result.GetFirst();

  auto second_type_result = second->Typecheck(env);
  if (!second_type_result) {
    return second_type_result;
  }
  auto *second_type = second_type_result.GetFirst();

  if (first_type != second_type) {
    std::string errmsg =
        std::string("first alternative has type: ") + first_type->ToString() +
        ", second alternative has type: " + second_type->ToString();
    return {Error(Error::Code::CondBodyExprTypeMismatch, GetLoc(), errmsg)};
  }

  return {first_type};
}

// the outline of generating the code for a conditional expression
// was retrieved from here:
//  llvm.org/docs/tutorial/MyFirstLanguageFrontend/LangImpl05.html
auto Conditional::Codegen(const Environment &env) const
    -> Outcome<llvm::Value *, Error> {
  assert(GetType() != nullptr);
  auto *result_type = GetType()->ToLLVM(env);

  auto test_codegen_result = test->Codegen(env);
  if (!test_codegen_result) {
    return test_codegen_result;
  }
  auto *test_value = test_codegen_result.GetFirst();

  if (env.current_function == nullptr) {
    FatalError("Cannot emit a conditional expression into global scope",
               __FILE__, __LINE__);
  }

  auto *then_BB =
      llvm::BasicBlock::Create(*env.context, "then", env.current_function);
  auto *else_BB = llvm::BasicBlock::Create(*env.context, "else");
  auto *merge_BB = llvm::BasicBlock::Create(*env.context, "merge");

  env.instruction_builder->CreateCondBr(test_value, then_BB, else_BB);
  env.instruction_builder->SetInsertPoint(then_BB);
  // emit the true branch
  auto first_codegen_result = first->Codegen(env);
  if (!first_codegen_result) {
    return first_codegen_result;
  }
  auto *first_value = first_codegen_result.GetFirst();
  env.instruction_builder->CreateBr(merge_BB);
  // the last basic block in the true branch may not
  // be the basic block we constructed for the true
  // branch, so update our reference to the true
  // branch to the actual last basic block.
  then_BB = env.instruction_builder->GetInsertBlock();

  env.current_function->getBasicBlockList().push_back(else_BB);
  env.instruction_builder->SetInsertPoint(else_BB);
  // emit the false branch
  auto second_codegen_result = second->Codegen(env);
  if (!second_codegen_result) {
    return second_codegen_result;
  }
  auto *second_value = second_codegen_result.GetFirst();
  env.instruction_builder->CreateBr(merge_BB);

  else_BB = env.instruction_builder->GetInsertBlock();

  env.current_function->getBasicBlockList().push_back(merge_BB);
  env.instruction_builder->SetInsertPoint(merge_BB);

  auto *phi_node = env.instruction_builder->CreatePHI(result_type, 2, "phi");

  phi_node->addIncoming(first_value, then_BB);
  phi_node->addIncoming(second_value, else_BB);
  return {phi_node};
}

} // namespace pink
