

#include "ast/While.h"

#include "aux/Environment.h"

namespace pink {
/*
auto While::TypecheckV(const Environment &env) const -> Outcome<Type *, Error> {
  auto test_getype_result = test->Typecheck(env);
  if (!test_getype_result) {
    return test_getype_result;
  }

  auto *bool_t = env.types->GetBoolType();

  if (bool_t != test_getype_result.GetFirst()) {
    std::string errmsg = std::string("test expression has type: ") +
                         test_getype_result.GetFirst()->ToString();
    return {Error(Error::Code::WhileTestTypeMismatch, test->GetLoc(), errmsg)};
  }

  auto body_getype_result = body->Typecheck(env);

  if (body_getype_result) {
    return {env.types->GetNilType()};
  }
  return body_getype_result;
}
*/
/*
auto While::Codegen(const Environment &env) const
    -> Outcome<llvm::Value *, Error> {
  assert(GetType() != nullptr);
  assert(env.current_function != nullptr);

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
  // the IR.
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
  env.current_function->insert(env.current_function->end(), body_BB);

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
  env.current_function->insert(env.current_function->end(), after_BB);

  // 10) The type of While is Nil, because a While Expression always
  // returns nil.
  // #NOTE: we could return the result of evaluating the body as the
  //        result of a while expression, and that would be natural,
  //        except that the case where the loop never runs asks
  //        the question of what to return. to me the obvious answer
  //        is the default construction of the value. However that
  //        also seems like a bad idea.
  return {env.instruction_builder->getFalse()};
}
*/
} // namespace pink
