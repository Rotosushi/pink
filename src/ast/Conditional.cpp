
#include "ast/Conditional.h"

#include "aux/Environment.h"

namespace pink {
  Conditional::Conditional(Location loc, std::unique_ptr<Ast> test, std::unique_ptr<Ast> first, std::unique_ptr<Ast> second)
    : Ast(Ast::Kind::Conditional, loc), test(std::move(test)), first(std::move(first)), second(std::move(second))
  {

  }

  Conditional::~Conditional()
  {

  }

  bool Conditional::classof(const Ast* ast)
  {
    return ast->getKind() == Ast::Kind::Conditional;
  }

  std::unique_ptr<Ast> Conditional::Clone()
  {
    return std::make_unique<Conditional>(loc, test->Clone(), first->Clone(), second->Clone());
  }

  std::string Conditional::ToString()
  {
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
  Outcome<Type*, Error> Conditional::GetypeV(const Environment& env)
  {
    Outcome<Type*, Error> test_type_result = test->Getype(env);

    if (!test_type_result)
      return test_type_result;

    Type* bool_ty = env.types->GetBoolType();

    if (bool_ty != test_type_result.GetOne())
    {
      std::string errmsg = std::string("condition has type: ")
                         + test_type_result.GetOne()->ToString();
      return Outcome<Type*, Error> (Error(Error::Code::CondTestExprTypeMismatch, test->GetLoc(), errmsg));
    }

    Outcome<Type*, Error> first_type_result = first->Getype(env);

    if (!first_type_result)
      return first_type_result;

    Outcome<Type*, Error> second_type_result = second->Getype(env);

    if (!second_type_result)
      return second_type_result;

    if (first_type_result.GetOne() != second_type_result.GetOne())
    {
      std::string errmsg = std::string("first alternative has type: ")
                         + first_type_result.GetOne()->ToString()
                         + ", second alternative has type: "
                         + second_type_result.GetOne()->ToString();
      return Outcome<Type*, Error> (Error(Error::Code::CondBodyExprTypeMismatch, loc, errmsg));
    }

    return Outcome<Type*, Error>(first_type_result.GetOne());
  }


  Outcome<llvm::Value*, Error> Conditional::Codegen(const Environment& env)
  {
    // the steps outlined here are taken from 
    //  llvm.org/docs/tutorial/MyFirstLanguageFrontend/LangImpl05.html
    // the outline of this procedure is as follows
    // 1) emit the code for the test expression and hold it's result
    // 2) construct the basicblocks for each alternative body term and for the
    //    merge of the two alternative values.
    // 3) construct a conditional branch instruction based on the value of the 
    //    test expression, passing in the two basicblocks for the alternative 
    //    expressions as the targets for the branch instruction.
    // 3) change the insert point to the first alternative to the 
    //    basicblock we constructed for the same, before emitting the
    //    code for the same. once done emitting the code for the first 
    //    alternative, emit an unconditional brach instruction to the 
    //    merge basicblock.
    // 4) do the same for the second alternative.
    // 5) change the insert point to the merge basicblock and emit a 
    //    PHI instruction which handles merging the two result values 
    //    of the first and second alternatives before returning the 
    //    PHI node as the result value of the entire conditional 
    //    expression.
    
    // get the types of all the expressions
    Outcome<Type*, Error> test_getype_result = test->Getype(env);

    if (!test_getype_result)
      return Outcome<llvm::Value*, Error>(test_getype_result.GetTwo());

    Outcome<llvm::Type*, Error> test_type_result = test_getype_result.GetOne()->Codegen(env);

    if (!test_type_result)
      return Outcome<llvm::Value*, Error>(test_type_result.GetTwo());

    // since we are inside of the codegen routine, then we can assume 
    // that the type of this expression is good, which means the types 
    // of the first and second alternative are identical, which means 
    // we technically only need to retrieve the type of one of either 
    // of them to know the type of both here.
    Outcome<Type*, Error> first_getype_result = first->Getype(env);

    if (!first_getype_result)
      return Outcome<llvm::Value*, Error>(first_getype_result.GetTwo()); 

    Outcome<llvm::Type*, Error> first_type_result = first_getype_result.GetOne()->Codegen(env);

    if (!first_type_result)
      return Outcome<llvm::Value*, Error>(first_type_result.GetTwo());

    // 1) emit the code for the test expression
    // we want to generate the code for the test expression within the 
    // basic block we are already within when we attempt to codegen the 
    // conditional expression.
    Outcome<llvm::Value*, Error> test_value_result = test->Codegen(env);

    if (!test_value_result)
      return test_value_result;

    // 2) emit the conditional branch instruction
    if (env.current_function == nullptr)
    {
      FatalError("Cannot emit a conditional expression without a basic block available to insert into!", __FILE__, __LINE__);
      return Outcome<llvm::Value*, Error>(nullptr); // FatalError is marked [[noreturn]] so why does the compiler complain about not all code paths returning a value?
    }

    // 3) construct the basic blocks we will be emitting code into
    // we want the first alternatives basic block to appear immediately after
    // the current basicblock we are at within the function.
    llvm::BasicBlock* then_BB = llvm::BasicBlock::Create(*env.context, "then", env.current_function);
    // we want the second and merge basic blocks to appear after the first
    // alternatives basic block, which given that the basic blocks which exist 
    // might themselves be modified by emitting the code for the first
    // alternatives basic block means we must wait to append the next two basic 
    // blocks until after we have already emitted the code for the first
    // alternatives basic block.
    llvm::BasicBlock* else_BB = llvm::BasicBlock::Create(*env.context, "else");
    llvm::BasicBlock* merge_BB = llvm::BasicBlock::Create(*env.context, "merge");

    // 4) emit the conditional branch instruction
    env.instruction_builder->CreateCondBr(test_value_result.GetOne(), then_BB, else_BB);

    // 5) emit the code for the first alternative into the basic block
    //    constructed for the first alternative
    // 
    // #NOTE: we do not save/restore the current insert point 
    // because any code appearing after the conditional expression 
    // must appear after the last instruction this function emits into 
    // the merge block that it constructs. so that any code appearing 
    // after the conditional expression is evaluated after we evaluate 
    // the conditional. if we did save/restore we would emit code 
    // that appeared after the conditional expression textually, after 
    // the conditional branch instruction semantically, making all of 
    // it unreachable code.
    env.instruction_builder->SetInsertPoint(then_BB);

    // #NOTE: if the compiler truly does fail here, doesn't it leave 
    // the compiler in an 'inoperable' state? that is, with a bunch 
    // of half connected basic blocks that are invalid from llvm's 
    // perspective?
    //
    // this i think is something that needs to be addressed when we consider 
    // attempting to parse more of an input file past the first error given.

    Outcome<llvm::Value*, Error> first_codegen_result = first->Codegen(env);

    if (!first_codegen_result)
      return first_codegen_result;

    // after the code appearing within the then block, we want to 
    // branch to the merge block unconditionally
    env.instruction_builder->CreateBr(merge_BB);

    // then since the codegen for the first alternative may change 
    // the current block, (such as in the case of a nested conditional
    // expression) and thus make the 'then_BB' we constructed not actually 
    // point to the relevant basic block for the merging of values,
    // we update the then_BB for the later construction 
    // of the PHI node. 
    then_BB = env.instruction_builder->GetInsertBlock();

    // 6) emit the code for the second alternative into the basic block 
    //    constructed for the second alternative
    //
    // #NOTE: we add the second alternatives basic block to the current
    // functions list of basic blocks here, after emitting the code for the 
    // first alternatives basic block precisely because emitting the code 
    // for the first alternative may modify the basic block list itself.
    env.current_function->getBasicBlockList().push_back(else_BB);
    // set the current insertion point to the relevant basic block
    env.instruction_builder->SetInsertPoint(else_BB);

    Outcome<llvm::Value*, Error> second_codegen_result = second->Codegen(env);

    if (!second_codegen_result)
      return second_codegen_result;
    // construct the required unconditional branch to the merge block
    env.instruction_builder->CreateBr(merge_BB);
    // since codegen can modify the basic block being inserted into, we 
    // must update the else_BB to be the relevant one.
    else_BB = env.instruction_builder->GetInsertBlock();

    // 7) emit the code for the merge block
    env.current_function->getBasicBlockList().push_back(merge_BB);
    env.instruction_builder->SetInsertPoint(merge_BB);

    llvm::PHINode* pn = env.instruction_builder->CreatePHI(first_type_result.GetOne(), 2, "mergeval");

    pn->addIncoming(first_codegen_result.GetOne(), then_BB);
    pn->addIncoming(second_codegen_result.GetOne(), else_BB);
    return Outcome<llvm::Value*, Error>(pn);
  }

}



