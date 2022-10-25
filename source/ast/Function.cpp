
#include "ast/Function.h"

#include "aux/Environment.h"

#include "kernel/Cast.h"
#include "kernel/StoreAggregate.h"
#include "kernel/SysExit.h"

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

void Function::Accept(AstVisitor *visitor) const { visitor->Visit(this); }

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
  // first, construct a bunch of false bindings with the correct Types.
  // such that we can properly Type the body, which presumably uses said
  // bindings.

  // next, construct a new environment, where the SymbolTable is now the
  // SymbolTable of this function, such that Lookup will resolve the new
  // bindings constructed

  // then type the body of the function.

  // even if the body returned an error, first, clean up the false bindings
  // so that codegen can properly bind the arguments to their actual
  // llvm::Value*s

  // construct a Function Type* from the types of the
  // arguments and the type we got from the body.

  // construct the false bindings, with no real llvm::Value*
  // note that we do not use the false_bindings structure
  // within the ENV to record the names of the arguments
  // being bound here. No sense duplicating the name when
  // it is already stored in the arguments structure.
  for (const auto &pair : arguments) {
    bindings->Bind(pair.first, pair.second, nullptr);
  }

  // build a new environment around the functions symboltable, such that we have
  // syntactic scoping.
  auto inner_env = std::make_unique<Environment>(env, bindings);

  // type the body with respect to the inner_env
  Outcome<Type *, Error> body_result = body->Typecheck(*inner_env);

  // remove the false bindings before returning;
  for (const auto &pair : arguments) {
    bindings->Unbind(pair.first);
  }

  // also remove any false bindings that any bind
  // instructions within this functions scope
  // constructed to declare local variables.
  for (InternedString fbnd : *inner_env->false_bindings) {
    inner_env->bindings->Unbind(fbnd);
  }

  inner_env->false_bindings->clear();

  if (!body_result) {
    return body_result;
  }

  // build the function type to return it.
  // This is an interesting bug. since we do the performant
  // thing here, and allocate space for all arguments within
  // the newly constructed arg_types vector. the calls to
  // push_back or emplace_back are pushing to the end, past
  // where the preallocated space is. so I suppose we must use
  // an index into the vector to get the assignment semantics
  // we need.
  std::vector<Type *> arg_types(arguments.size());
  size_t idx = 0;

  for (const auto &pair : arguments) {
    arg_types.at(idx) = pair.second;
    idx++;
  }

  auto *func_ty = env.types->GetFunctionType(body_result.GetFirst(), arg_types);

  // ensure that the rest of this compilation unit knows about this function.
  env.bindings->Bind(name, func_ty, nullptr);
  env.false_bindings->push_back(name);

  return {func_ty};
}

auto Function::Codegen(const Environment &env) const
    -> Outcome<llvm::Value *, Error> {

  bool is_main = false;
  if (strcmp(name, "main") == 0) {
    is_main = true;
  }

  assert(GetType() != nullptr);

  Outcome<llvm::Type *, Error> ty_codegen_result = GetType()->Codegen(env);

  if (!ty_codegen_result) {
    return {ty_codegen_result.GetSecond()};
  }

  auto *p_fn_ty = llvm::cast<pink::FunctionType>(GetType());

  auto *fn_ret_ty = p_fn_ty->result->Codegen(env);

  llvm::FunctionType *function_ty = nullptr;
  llvm::Function *function = nullptr;
  if (is_main) {
    auto *main_fn_ty = env.types->GetFunctionType(env.types->GetVoidType(),
                                                  p_fn_ty->arguments);

    function_ty = llvm::cast<llvm::FunctionType>(main_fn_ty->Codegen(env));

    function = llvm::Function::Create(
        function_ty, llvm::Function::ExternalLinkage, name, *env.llvm_module);

  } else {
    function_ty = llvm::cast<llvm::FunctionType>(ty_codegen_result.GetFirst());

    function = llvm::Function::Create(
        function_ty, llvm::Function::ExternalLinkage, name, *env.llvm_module);
  }

  auto attr_result =
      CodegenParameterAttributes(env, function, function_ty, p_fn_ty);

  if (!attr_result) {
    return attr_result;
  }

  auto *entryblock = llvm::BasicBlock::Create(*env.context, "entry", function);
  auto entrypoint = entryblock->getFirstInsertionPt();

  auto local_builder =
      std::make_shared<llvm::IRBuilder<>>(entryblock, entrypoint);

  auto local_env =
      std::make_unique<Environment>(env, bindings, local_builder, function);

  CodegenArgumentInit(*local_env, function, p_fn_ty);

  auto body_result = body->Codegen(*local_env);

  if (!body_result) {
    return body_result;
  }

  if (is_main) {
    auto main_ret = CodegenMainReturn(*local_env, body_result.GetFirst());

    if (!main_ret) {
      return main_ret;
    }
  } else {
    if (fn_ret_ty->isSingleValueType()) {
      local_builder->CreateRet(body_result.GetFirst());
    } else {
      StoreAggregate(fn_ret_ty, function->getArg(0), body_result.GetFirst(),
                     *local_env);
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

auto Function::CodegenMainReturn(const Environment &env,
                                 llvm::Value *body_value)
    -> Outcome<llvm::Value *, Error> {
  SysExit(body_value, env);
  // #NOTE: #TODO:
  // if this line is not here, the program segfaults when
  // we try and print the program to a file. I am really unsure
  // why this is, and the segfault happens deep in llvm.
  // so, figure out why this happens and what we can do to
  // remove this line. this is in fact always dead code,
  // so i am initailly unsure why this is even necessary.
  env.instruction_builder->CreateRetVoid();
  return {nullptr};
}

auto Function::CodegenParameterAttributes(
    const Environment &env, llvm::Function *function,
    const llvm::FunctionType *function_type,
    const pink::FunctionType *p_function_type)
    -> Outcome<llvm::Value *, Error> {

  auto *fn_ret_ty = p_function_type->result->Codegen(env);
  /* find out if we need to
   * add the sret parameter attribute to a parameter
   * of the function.
   *
   * this is within a conditional because if it true, then
   * the parameter list holds the return value, so the parameter
   * list is one element larger that the pink::FunctionType says.
   * so we have to use different offsets to get to each argument.
   */
  if (!fn_ret_ty->isSingleValueType() &&
      function_type->getReturnType()->isVoidTy()) {
    llvm::AttrBuilder ret_attr_builder(*env.context);
    // since the return type is not a single value type, we know that
    // the function type associated with this function will have an
    // actual return type of void, and the first parameter will be the
    // return value of this function, knowing that, we then need to add
    // the sret(<ty>) attribute to this parameter attribute
    ret_attr_builder.addStructRetAttr(fn_ret_ty);
    function->addParamAttrs(0, ret_attr_builder);
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
    for (size_t i = 1; i < function_type->getNumParams(); i++) {
      llvm::AttrBuilder param_attr_builder(*env.context);

      llvm::Type *param_ty = p_function_type->arguments[i]->Codegen(env);

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
    for (size_t i = 0; i < function_type->getNumParams(); i++) {
      llvm::AttrBuilder param_attr_builder(*env.context);
      llvm::Type *param_ty = p_function_type->arguments[i]->Codegen(env);

      if ((!param_ty->isVoidTy()) && !param_ty->isSingleValueType()) {
        param_attr_builder.addByValAttr(param_ty);
        function->addParamAttrs(i, param_attr_builder);
      }
    }
  }
  return {nullptr};
}

void Function::CodegenArgumentInit(
    const Environment &env, const llvm::Function *function,
    const pink::FunctionType *p_function_type) const {

  // allocate space for each argument
  std::vector<std::pair<llvm::Value *, llvm::Value *>> arg_ptrs;
  for (size_t i = 0; i < function->arg_size(); i++) {
    llvm::Argument *arg = function->getArg(i);
    llvm::Value *arg_ptr = env.instruction_builder->CreateAlloca(
        arg->getType(), env.data_layout.getAllocaAddrSpace(), nullptr,
        arg->getName());

    arg_ptrs.emplace_back(arg, arg_ptr);
  }

  // initialize each argument
  for (size_t i = 0; i < arguments.size(); i++) {
    llvm::Argument *arg = function->getArg(i);

    llvm::Type *arg_ty = arg->getType();

    if (arg_ty->isSingleValueType()) {
      env.instruction_builder->CreateStore(arg_ptrs[i].first,
                                           arg_ptrs[i].second, false);
    } else {
      StoreAggregate(arg_ty, arg_ptrs[i].second, arg_ptrs[i].first, env);
    }

    env.bindings->Bind(this->arguments[i].first, p_function_type->arguments[i],
                       arg_ptrs[i].second);
  }
}

} // namespace pink