
#include "ast/Function.h"

#include "aux/Environment.h"

#include "kernel/Cast.h"
#include "kernel/StoreAggregate.h"
#include "kernel/StoreValue.h"
#include "kernel/sys/SysExit.h"

#include "support/LLVMErrorToString.h"

#include "visitor/AstVisitor.h"

#include "llvm/Support/raw_os_ostream.h"

#include "llvm/IR/Verifier.h"

namespace pink {
Function::Function(const Location &location, const InternedString name,
                   std::vector<std::pair<InternedString, Type *>> arguments,
                   std::unique_ptr<Ast> body, SymbolTable *outer_scope)
    : Ast(Ast::Kind::Function, location), name(name),
      arguments(std::move(arguments)), body(std::move(body)),
      bindings(std::make_shared<SymbolTable>(outer_scope)) {}

void Function::Accept(const ConstAstVisitor *visitor) const {
  visitor->Visit(this);
}

auto Function::classof(const Ast *ast) -> bool {
  return ast->GetKind() == Ast::Kind::Function;
}

/*
'fn' name '(' (args[0].first ':' args[0].second->ToString() (',')?)+ ')\n'
'{' '\n'
        body->ToString() '\n'
'}' '\n'
*/
auto Function::ToString() const -> std::string {
  std::string result;

  result += std::string("fn ") + name;

  result += " (";

  size_t len = arguments.size();

  for (size_t i = 0; i < len; i++) {
    result += arguments[i].first;
    result += ": " + arguments[i].second->ToString();

    if (i < (len - 1)) {
      result += ", ";
    }
  }

  result += ") {" + body->ToString() + "}";

  return result;
}

/*
                          env |- a0 : T0, a1 : T1, ..., an : Tn, body : Tb
 -----------------------------------------------------------------------------
  env |- 'fn' name '(' a0: T0, a1: T1, ..., an : Tn ')' '{' body '}
    : T0 -> T1 -> ... -> Tn -> Tb

If a function takes an argument which is not a single value type, then we need
to mark that parameter as byval(<ty>). where <ty> is the pointee type,
additionalily, any non single value type must
then be passed via a pointer in that parameter, which means promoting the type
to that of a pointer within the argument.

If a function returns an object which is not a sigle value type, then we need to
modify the underlying function type to return void, and add an extra parameter
which has type, pointer to the non single value type, and which has the
parameter attribute 'sret(<ty>)' where <ty> is the pointee type.

So, I don't think that a pink::FunctionType needs to be modified in any way,
just it's mapping to a llvm::FunctionType, however call sites must also be aware
of the difference, because the sret(<ty>) parameter must be allocated by the
caller before we call the function.
*/
auto Function::TypecheckV(const Environment &env) const
    -> Outcome<Type *, Error> {
  for (const auto &pair : arguments) {
    bindings->Bind(pair.first, pair.second, nullptr);
  }

  auto local_env = Environment::NewLocalEnv(env, bindings);

  // type the body with respect to the local_env
  auto body_result = body->Typecheck(*local_env);

  // remove the false bindings before returning;
  for (const auto &pair : arguments) {
    bindings->Unbind(pair.first);
  }

  for (InternedString fbnd : *(local_env->false_bindings)) {
    local_env->bindings->Unbind(fbnd);
  }
  local_env->false_bindings->clear();

  if (!body_result) {
    return body_result;
  }

  std::vector<Type *> arg_types(arguments.size());

  auto ToType = [](const std::pair<InternedString, Type *> &arg) -> Type * {
    return arg.second;
  };

  std::transform(arguments.begin(), arguments.end(), arg_types.begin(), ToType);

  auto *func_ty = env.types->GetFunctionType(body_result.GetFirst(), arg_types);

  // ensure that the rest of this compilation unit knows about this function.
  env.bindings->Bind(name, func_ty, nullptr);
  env.false_bindings->push_back(name);

  return {func_ty};
}

auto Function::Codegen(const Environment &env) const
    -> Outcome<llvm::Value *, Error> {
  assert(GetType() != nullptr);
  bool is_main = false;
  if (strcmp(name, "main") == 0) { // only run strcmp once
    is_main = true;
  }

  auto *pink_function_type = llvm::cast<pink::FunctionType>(GetType());

  auto *llvm_return_type = pink_function_type->result->ToLLVM(env);

  llvm::FunctionType *llvm_function_type = nullptr;
  llvm::Function *function = [&]() {
    // #RULE main returns via a call to sys_exit.
    // so the llvm function type returns void
    if (is_main) {
      auto *main_function_type = env.types->GetFunctionType(
          env.types->GetVoidType(), pink_function_type->arguments);

      llvm_function_type =
          llvm::cast<llvm::FunctionType>(main_function_type->ToLLVM(env));

      return llvm::Function::Create(llvm_function_type,
                                    llvm::Function::ExternalLinkage, name,
                                    *env.llvm_module);
    }

    llvm_function_type =
        llvm::cast<llvm::FunctionType>(pink_function_type->ToLLVM(env));

    return llvm::Function::Create(llvm_function_type,
                                  llvm::Function::ExternalLinkage, name,
                                  *env.llvm_module);
  }();

  CodegenParameterAttributes(env, function, llvm_function_type,
                             pink_function_type);

  auto *entryblock = llvm::BasicBlock::Create(
      *env.context, std::string(name) + "_entry", function);
  auto entrypoint = entryblock->getFirstInsertionPt();

  auto local_builder =
      std::make_shared<llvm::IRBuilder<>>(entryblock, entrypoint);

  auto local_env =
      Environment::NewLocalEnv(env, bindings, local_builder, function);

  CodegenArgumentInit(*local_env, function, pink_function_type);

  auto body_result = body->Codegen(*local_env);
  if (!body_result) {
    return body_result;
  }

  if (is_main) {
    CodegenMainReturn(*local_env, body_result.GetFirst());
  } else {
    if (llvm_return_type->isSingleValueType()) {
      local_builder->CreateRet(body_result.GetFirst());
    } else {
      StoreAggregate(llvm_return_type, function->getArg(0),
                     body_result.GetFirst(), *local_env);
    }
  }

  std::string buffer;
  llvm::raw_string_ostream out(buffer);
  if (llvm::verifyFunction(*function, &out)) {
    FatalError(buffer, __FILE__, __LINE__);
  }

  env.bindings->Bind(name, GetType(), function);

  return {function};
}

void Function::CodegenMainReturn(const Environment &env,
                                 llvm::Value *body_value) {
  CodegenSysExit(body_value, env);
  // even though we call sysexit to return from our
  // program, llvm considers the main function
  // ill-formed without this return statement.
  env.instruction_builder->CreateRetVoid();
}

void Function::CodegenParameterAttributes(
    const Environment &env, llvm::Function *function,
    const llvm::FunctionType *llvm_function_type,
    const pink::FunctionType *pink_function_type) {

  auto *result_type = pink_function_type->result->ToLLVM(env);
  /* find out if we need to
   * add the sret parameter attribute to a parameter
   * of the function.
   *
   * this is within a conditional because if it true, then
   * the parameter list holds the return value, so the parameter
   * list is one element larger that the pink::FunctionType says.
   * so we have to use different offsets to get to each argument.
   */
  if (!result_type->isSingleValueType()) {
    assert(llvm_function_type->getReturnType()->isVoidTy());

    llvm::AttrBuilder result_attribute_builder(*env.context);
    // since the return type is not a single value type, we know that
    // the function type associated with this function will have an
    // actual return type of void, and the first parameter will be the
    // return value of this function, knowing that, we then need to add
    // the sret(<ty>) attribute to the first parameter attribute
    result_attribute_builder.addStructRetAttr(result_type);
    function->addParamAttrs(0, result_attribute_builder);
    /*
     *  Now all we need to do is add any byval(<ty>) attributes
     *  to any argument which needs it. We must be careful to
     *  mention however, that in the translation from pink::FunctionType
     *  to llvm::FunctionType we also promote any non singleValueType to
     *  an opaque pointer type. So we have to use the arguments within the
     *  Function to know the type. as otherwise we couldn't tell the
     *  difference between a pointer that needed the byval(<ty>) attribute,
     *  and a pointer that did not.
     */
    for (size_t i = 1; i < llvm_function_type->getNumParams(); i++) {
      llvm::AttrBuilder param_attr_builder(*env.context);

      llvm::Type *param_ty = pink_function_type->arguments[i - 1]->ToLLVM(env);

      // since this type is not a single value type, this parameter needs
      // the byval(<ty>) parameter attribute
      if ((!param_ty->isVoidTy()) && !param_ty->isSingleValueType()) {
        param_attr_builder.addByValAttr(param_ty);
        function->addParamAttrs(i, param_attr_builder);
      }
    }
  } else {
    /*
     *  all we need to do here is add any byval(<ty>) attributes
     *  to any argument which needs it.
     */
    for (size_t i = 0; i < llvm_function_type->getNumParams(); i++) {
      llvm::AttrBuilder param_attr_builder(*env.context);
      llvm::Type *param_ty = pink_function_type->arguments[i]->ToLLVM(env);

      if ((!param_ty->isVoidTy()) && !param_ty->isSingleValueType()) {
        param_attr_builder.addByValAttr(param_ty);
        function->addParamAttrs(i, param_attr_builder);
      }
    }
  }
}

void Function::CodegenArgumentInit(
    const Environment &env, const llvm::Function *function,
    const pink::FunctionType *pink_function_type) const {

  // allocate space for each argument
  auto allocate = [&env](const llvm::Argument &arg) {
    return env.instruction_builder->CreateAlloca(
        arg.getType(), env.data_layout.getAllocaAddrSpace(), nullptr,
        arg.getName());
  };

  std::vector<llvm::AllocaInst *> arg_allocas(function->arg_size());

  std::transform(function->arg_begin(), function->arg_end(),
                 arg_allocas.begin(), allocate);

  // initialize each argument
  for (size_t i = 0; i < arguments.size(); i++) {
    llvm::Argument *arg = function->getArg(i);

    llvm::Type *arg_ty = arg->getType();

    StoreValue(arg_ty, arg_allocas[i], arg, env);

    env.bindings->Bind(arguments[i].first, pink_function_type->arguments[i],
                       arg_allocas[i]);
  }
}

} // namespace pink
