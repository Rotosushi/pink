

#include "ast/While.h"

#include "aux/Environment.h"

#include "visitor/AstVisitor.h"

namespace pink {
While::While(const Location &location, std::unique_ptr<Ast> test,
             std::unique_ptr<Ast> body)
    : Ast(Ast::Kind::While, location), test(std::move(test)),
      body(std::move(body)) {}

void While::Accept(AstVisitor *visitor) const { visitor->Visit(this); }

auto While::classof(const Ast *ast) -> bool {
  return ast->GetKind() == Ast::Kind::While;
}

auto While::ToString() const -> std::string {
  std::string result("while ");
  result += test->ToString();
  result += " do ";
  result += body->ToString();
  return result;
}

auto While::TypecheckV(const Environment &env) const -> Outcome<Type *, Error> {
  Outcome<Type *, Error> test_getype_result = test->Typecheck(env);

  if (!test_getype_result) {
    return test_getype_result;
  }

  Type *bool_t = env.types->GetBoolType();

  if (bool_t != test_getype_result.GetFirst()) {
    std::string errmsg = std::string("test expression has type: ") +
                         test_getype_result.GetFirst()->ToString();
    return {Error(Error::Code::WhileTestTypeMismatch, test->GetLoc(), errmsg)};
  }

  Outcome<Type *, Error> body_getype_result = body->Typecheck(env);

  if (body_getype_result) {
    return {env.types->GetVoidType()};
  }
  return body_getype_result;
}

auto While::Codegen(const Environment &env) const
    -> Outcome<llvm::Value *, Error> {
  assert(GetType() != nullptr);

  Outcome<llvm::Type *, Error> test_type_result = GetType()->Codegen(env);

  if (!test_type_result) {
    return {test_type_result.GetSecond()};
  }

  Outcome<Type *, Error> body_getype_result = body->Typecheck(env);

  if (!body_getype_result) {
    return {body_getype_result.GetSecond()};
  }

  Outcome<llvm::Type *, Error> body_type_result =
      body_getype_result.GetFirst()->Codegen(env);

  if (!body_type_result) {
    return {body_type_result.GetSecond()};
  }

  if (env.current_function == nullptr) {
    FatalError("Cannot emit a While loop without a basic block available to "
               "insert into!",
               __FILE__, __LINE__);
  }

  // 1) construct all basic blocks
  // NOTE: we do not insert the 'body' or 'after' basic blocks to handle
  // potentially recursive loops and conditionals. if we could garuntee no
  // recursive loops or conditionals then we could safely insert all three basic
  // blocks right here. However since we can expect recursive conditionals and
  // loops, we have to insert the next basic block of this loop -after- the
  // final basic block of the potential loop or conditional appearing in this
  // while loops body or conditional. In the case where there is no recursion,
  // this code safely inserts all three basic blocks one after another, just as
  // would happen if we were to insert all three here.
  llvm::BasicBlock *test_BB = llvm::BasicBlock::Create(
      *env.context, "loop_condition", env.current_function);
  llvm::BasicBlock *body_BB =
      llvm::BasicBlock::Create(*env.context, "loop_body");
  llvm::BasicBlock *after_BB =
      llvm::BasicBlock::Create(*env.context, "loop_end");

  // 2) emit the fallthrough brach instruction
  // NOTE: it is -required- to emit a 'fallthrough' branch instruction
  // to connect these two basic blocks. otherwise llvm will not compile
  // the code! because it does not assume adjacency within the list of
  // basic blocks to imply adjacency of the code within those basic blocks.
  env.instruction_builder->CreateBr(test_BB);

  // 3) set up the ir builder to emit instructions into the test basic block
  env.instruction_builder->SetInsertPoint(test_BB);

  // 4) emit the test code into the test basic block
  Outcome<llvm::Value *, Error> test_codegen_result = test->Codegen(env);

  if (!test_codegen_result) {
    return test_codegen_result;
  }
  // 5) emit the conditional branch instruction into the test basic block
  env.instruction_builder->CreateCondBr(test_codegen_result.GetFirst(), body_BB,
                                        after_BB);

  // 6) set up for emitting the body code into the body basic block
  env.instruction_builder->SetInsertPoint(body_BB);
  env.current_function->getBasicBlockList().push_back(body_BB);
  // 7) emit the body code
  Outcome<llvm::Value *, Error> body_codegen_result = body->Codegen(env);

  if (!body_codegen_result) {
    return body_codegen_result;
  }

  // 8) emit the uncondition branch back up to the test basic block
  //    which makes this a loop.
  env.instruction_builder->CreateBr(test_BB);

  // 9) set up the ir builder to emit code after the while loop
  env.instruction_builder->SetInsertPoint(after_BB);
  env.current_function->getBasicBlockList().push_back(after_BB);

  // 10) return the results of the while loop.
  // #NOTE: we could return the result of evaluating the body as the
  //        result of a while expression, and that would be natural,
  //        except that the case where the loop never runs asks
  //        the question of what to return. to me the obvious answer
  //        is the default construction of the value. However that means
  //        that any value that the while loop returns must
  //        be default constructable from then on. which may
  //        or may not be weird for users.
  return {env.instruction_builder->getFalse()};
}
} // namespace pink
