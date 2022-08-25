

#include "ast/While.h"

#include "aux/Environment.h"

namespace pink {
  While::While(Location l, std::unique_ptr<Ast> test, std::unique_ptr<Ast> body)
    : Ast(Ast::Kind::While, l), test(std::move(test)), body(std::move(body))
  {

  }

  While::~While()
  {
  
  }

  bool While::classof(const Ast* ast)
  {
    return ast->getKind() == Ast::Kind::While;
  }

  std::unique_ptr<Ast> While::Clone()
  {
    return std::make_unique<While>(loc, test->Clone(), body->Clone());
  }

  std::string While::ToString()
  {
    std::string result("while ");
    result += test->ToString();
    result += " do ";
    result += body->ToString();
    return result;
  }

  Outcome<Type*, Error> While::GetypeV(std::shared_ptr<Environment> env)
  {
    Outcome<Type*, Error> test_getype_result = test->Getype(env);

    if (!test_getype_result)
      return test_getype_result;

    Type* bool_t = env->types->GetBoolType();

    if (bool_t != test_getype_result.GetOne())
    {
      return Outcome<Type*, Error>(Error(Error::Code::WhileTestTypeMismatch, test->GetLoc()));
    }

    Outcome<Type*, Error> body_getype_result = body->Getype(env);

    if (body_getype_result)
    {
      return Outcome<Type*, Error>(env->types->GetNilType());
    }
    else 
    {
      return body_getype_result;
    }
  }

  Outcome<llvm::Value*, Error> While::Codegen(std::shared_ptr<Environment> env)
  {
    // the general outline of this procedure is as follows:
    // 1) construct the basic block which represents the body of the loop,
    //    the basic block which represents the test expression, and
    //    the basic block which represents the code after the loop.
    // 2) emit a fallthrough unconditional branch instruction into 
    //    the current basic block which branches to the basic block 
    //    representing the test.
    // 3) modify the ir builder to point to the test expression basic block,
    //    and then emit the test expression, and then emit a conditional branch to
    //    either the loop body, or the code after the loop.
    // 4) modify the ir builder to point to the body expression basic block
    //    and emit the code for the body of the loop, then emit an
    //    unconditional branch to the test expression basic block.
    //
    // 5) then, since we are finished emitting the loop,
    //    modify the ir builder to point to the basic block representing the 
    //    code after the loop
    //
    //
    Outcome<Type*, Error> test_getype_result = test->Getype(env);

    if (!test_getype_result)
      return Outcome<llvm::Value*, Error>(test_getype_result.GetTwo());

    Outcome<llvm::Type*, Error> test_type_result = test_getype_result.GetOne()->Codegen(env);

    if (!test_type_result)
      return Outcome<llvm::Value*, Error>(test_type_result.GetTwo());
  
    Outcome<Type*, Error> body_getype_result = body->Getype(env);

    if (!body_getype_result)
      return Outcome<llvm::Value*, Error>(body_getype_result.GetTwo());

    Outcome<llvm::Type*, Error> body_type_result = body_getype_result.GetOne()->Codegen(env);

    if (!body_type_result)
      return Outcome<llvm::Value*, Error>(body_type_result.GetTwo());
 
    if (env->current_function == nullptr)
    {
      FatalError("Cannot emit a While loop without a basic block available to insert into!", __FILE__, __LINE__);
      return Outcome<llvm::Value*, Error>(nullptr); // suppress warning about not all code paths returning a value
    }

    // 1) construct all basic blocks
    llvm::BasicBlock* test_BB = llvm::BasicBlock::Create(*env->context, "test", env->current_function);
    llvm::BasicBlock* body_BB = llvm::BasicBlock::Create(*env->context, "loop");
    llvm::BasicBlock* after_BB = llvm::BasicBlock::Create(*env->context, "loop_end");
    
    // 2) emit the fallthrough brach instruction
    // it is very important to emit a 'fallthrough' branch instruction 
    // to connect these two basic blocks. otherwise llvm will not compile 
    // the code!
    env->builder->CreateBr(test_BB);

    // 3) set up the ir builder to emit instructions into the test basic block
    env->builder->SetInsertPoint(test_BB); 
    env->current_function->getBasicBlockList().push_back(test_BB);
    // 4) emit the test code
    Outcome<llvm::Value*, Error> test_codegen_result = test->Codegen(env);

    if (!test_codegen_result)
      return test_codegen_result;
    // 5) emit the conditional branch instruction
    env->builder->CreateCondBr(test_codegen_result.GetOne(), body_BB, after_BB);

    // 6) set up for emitting the body code
    env->builder->SetInsertPoint(body_BB);
    env->current_function->getBasicBlockList().push_back(body_BB);
    // 7) emit the body code
    Outcome<llvm::Value*, Error> body_codegen_result = body->Codegen(env);

    if (!body_codegen_result)
      return body_codegen_result;
    // 8) emit the uncondition branch back up to the test basic block
    env->builder->CreateBr(test_BB);

    // 9) set up the ir builder to emit code after the while loop
    env->builder->SetInsertPoint(after_BB);
    env->current_function->getBasicBlockList().push_back(after_BB);
    
    // return the results of the while loop.
    // #NOTE: we could return the result of evaluating the body as the 
    //        result of a while expression, and that would be natural,
    //        except that the case where the loop never runs asks 
    //        the question of default construction of any value.
    //        and now, any value that the while loop returns must 
    //        be default constructable from then on. which may
    //        or may not be weird for users.
    return Outcome<llvm::Value*, Error>(env->builder->getFalse()); 
  }
}
