
#include "ast/Application.h"

#include "aux/Environment.h"

#include "support/LLVMValueToString.h"

namespace pink {
  Application::Application(Location location, std::unique_ptr<Ast> callee, std::vector<std::unique_ptr<Ast>> arguments)
    : Ast(Ast::Kind::Application, location), callee(std::move(callee)), arguments(std::move(arguments))
  {

  }

  Application::~Application()
  {

  }

  bool Application::classof(const Ast* ast)
  {
    return ast->getKind() == Ast::Kind::Application;
  }

  std::unique_ptr<Ast> Application::Clone()
  {
    std::vector<std::unique_ptr<Ast>> args;

    for (std::unique_ptr<Ast>& arg : arguments)
    {
      args.emplace_back(arg->Clone());
    }

    return std::unique_ptr<Application>(new Application(loc, callee->Clone(), std::move(args)));
  }

  std::string Application::ToString()
  {
    std::string result;
    
    result += callee->ToString();
    result += " ";

    size_t i = 0;
    for (std::unique_ptr<Ast>& arg : arguments)
    {
      result += arg->ToString();
      
      if (i < (arguments.size() - 1))
        result += ", ";

      i++;
    }

    return result;
  }

  /*
   *  The type of an application is the return type of the
   *  callee, if and only if the number and types of each 
   *  argument matches the number and type of the callee's 
   *  arguments.
   *
   *        ENV |- 
   *          c : T0 -> T1 -> ... -> Tn -> Tr 
   *          a0 : Ta0, a1 : Ta1, ..., an : Tan,
   *          Ta0 = T0, Ta1 = T1, ..., Tan = Tn
   *   -------------------------------------------------
   *        ENV |- c a0 a1 ... an : Tr
   *
   *
   */
  Outcome<Type*, Error> Application::GetypeV(const Environment& env)
  {
    Outcome<Type*, Error> calleeTy = callee->Getype(env);

    if (!calleeTy)
      return calleeTy;

    // check that we have something we can call
    FunctionType* calleeFnTy = llvm::dyn_cast<FunctionType>(calleeTy.GetOne());

    if (calleeFnTy == nullptr)
    {
      Error error(Error::Code::TypeCannotBeCalled, loc, calleeTy.GetOne()->ToString());
      return Outcome<Type*, Error>(error);
    }

    // check that the number of arguments matches
    if (arguments.size() != calleeFnTy->arguments.size())
    {
      std::string errmsg = std::string("expected args: ") 
                         + std::to_string(calleeFnTy->arguments.size())
                         + ", actual args: "
                         + std::to_string(arguments.size());
      Error error(Error::Code::ArgNumMismatch, loc, errmsg);
      return Outcome<Type*, Error>(error);
    }
    
    std::vector<Type*> argTys;
    // check that each argument can be typed
    for (std::unique_ptr<Ast>& arg : arguments)
    {
      Outcome<Type*, Error> outcome = arg->Getype(env);

      if (!outcome)
        return Outcome<Type*, Error>(outcome.GetTwo());

      argTys.push_back(outcome.GetOne());
    }

    // chech that each arguments type matches
    for (size_t i = 0; i < arguments.size(); i++)
    {
      if (argTys[i] != calleeFnTy->arguments[i])
      {
        std::string errmsg = std::string("expected arg type: ")
                           + calleeFnTy->arguments[i]->ToString()
                           + ", actual arg type: "
                           + argTys[i]->ToString();
        Error error(Error::Code::ArgTypeMismatch, loc, errmsg);
        return Outcome<Type*, Error>(error);
      }
    }

    // return the functions result type as the type of the application term.
    return Outcome<Type*, Error>(calleeFnTy->result);
  }

  // The procedure is largely identical to Getype, in that we simply codegen
  // the callee and each argument, except for the fact that 
  // we have to call a few additional llvm functions to generate the correct
  // code. 
  Outcome<llvm::Value*, Error> Application::Codegen(const Environment& env)
  {
    // 1: codegen callee to retrieve the function pointer
    Outcome<llvm::Value*, Error> callee_result = callee->Codegen(env);

    if (!callee_result)
    {
      return Outcome<llvm::Value*, Error>(callee_result.GetTwo());
    }

    if (callee_result.GetOne() == nullptr)
    {
      FatalError("Function implementation was empty!", __FILE__, __LINE__);
    }

    llvm::Function* function = llvm::dyn_cast<llvm::Function>(callee_result.GetOne());

    if (function == nullptr)
    {
      std::string errmsg = std::string(", type was: ")
                         + LLVMValueToString(callee_result.GetOne());
      Error error(Error::Code::TypeCannotBeCalled, loc, errmsg);
      return Outcome<llvm::Value*, Error>(error);
    }


    // 2: codegen each argument to retrieve each llvm::Value* being passed as
    //    argument
    std::vector<llvm::Value*> arg_values;

    for (std::unique_ptr<Ast>& arg : arguments)
    {
      Outcome<llvm::Value*, Error> arg_result = arg->Codegen(env);

      if (!arg_result)
        return Outcome<llvm::Value*, Error>(arg_result.GetTwo());

      if (arg_result.GetOne() == nullptr)
        FatalError("The argument is nullptr!", __FILE__, __LINE__);

      arg_values.push_back(arg_result.GetOne());
    }
    
    llvm::CallInst* call = nullptr;
    // 3: codegen a CallInstruction to the callee passing in each argument.
    if (arg_values.size() == 0)
      call = env.instruction_builder->CreateCall(llvm::FunctionCallee(function));
    else
      call = env.instruction_builder->CreateCall(llvm::FunctionCallee(function), arg_values); 
    // 4: return the result of the call instruction as the llvm::Value* of this
    //    procedure
    return Outcome<llvm::Value*, Error>(call);
  }

}
