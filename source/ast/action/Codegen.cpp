#include "ast/action/Codegen.h"
#include "ast/action/ToString.h"
#include "ast/action/Typecheck.h"

#include "ast/All.h"

#include "ast/visitor/AstVisitor.h"
#include "visitor/VisitorResult.h"

#include "type/Type.h"
#include "type/action/ToLLVM.h"
#include "type/action/ToString.h"

#include "support/LLVMErrorToString.h"
#include "support/LLVMTypeToString.h"
#include "support/LLVMValueToString.h"

#include "aux/Environment.h"

#include "kernel/AllocateVariable.h"
#include "kernel/ArraySubscript.h"
#include "kernel/LoadValue.h"
#include "kernel/SliceSubscript.h"
#include "kernel/StoreValue.h"

#include "kernel/sys/SysExit.h"

#include "llvm/IR/Verifier.h"

namespace pink {
class CodegenVisitor
    : public ConstVisitorResult<CodegenVisitor, const Ast::Pointer &,
                                CodegenResult>,
      public ConstAstVisitor {
private:
  Environment &env;

public:
  void Visit(const Application *application) const noexcept override;
  void Visit(const Array *array) const noexcept override;
  void Visit(const Assignment *assignment) const noexcept override;
  void Visit(const Bind *bind) const noexcept override;
  void Visit(const Binop *binop) const noexcept override;
  void Visit(const Block *block) const noexcept override;
  void Visit(const Boolean *boolean) const noexcept override;
  void Visit(const Conditional *conditional) const noexcept override;
  void Visit(const Dot *dot) const noexcept override;
  void Visit(const Function *function) const noexcept override;
  void Visit(const Integer *integer) const noexcept override;
  void Visit(const Nil *nil) const noexcept override;
  void Visit(const Subscript *subscript) const noexcept override;
  void Visit(const Tuple *tuple) const noexcept override;
  void Visit(const Unop *unop) const noexcept override;
  void Visit(const Variable *variable) const noexcept override;
  void Visit(const While *loop) const noexcept override;

  CodegenVisitor(Environment &env) noexcept : env(env) {}
  ~CodegenVisitor() noexcept override                  = default;
  CodegenVisitor(const CodegenVisitor &other) noexcept = default;
  CodegenVisitor(CodegenVisitor &&other) noexcept      = default;
  auto operator=(const CodegenVisitor &other) noexcept
      -> CodegenVisitor                                             & = delete;
  auto operator=(CodegenVisitor &&other) noexcept -> CodegenVisitor & = delete;
};

/*
  The Codegen of an Ast::Application is a llvm::CallInst,
  this term represents the return value of the applied function,
  and so we can return it directly.
*/
void CodegenVisitor::Visit(const Application *application) const noexcept {
  auto callee_result = Compute(application->GetCallee(), this);
  if (!callee_result) {
    result = callee_result;
    return;
  }
  auto *callee_value = callee_result.GetFirst();

  auto *function     = llvm::dyn_cast<llvm::Function>(callee_value);
  if (function == nullptr) {
    std::string errmsg = "Callee value was [";
    errmsg             += LLVMValueToString(callee_value);
    errmsg             += "], not a [llvm::Function]";
    result = Error(Error::Code::TypeCannotBeCalled, application->GetLocation(),
                   errmsg);
    return;
  }

  std::vector<llvm::Value *> argument_values;

  for (const auto &argument : *application) {
    auto argument_result = Compute(argument, this);

    if (!argument_result) {
      result = argument_result;
      return;
    }
    /*
      This assert can only fail if we forget to unbind
      any false bindings in a scope before attempting
      to codegenerate that scope. However doesn't a
      symbol redeclaration error happen when we
      attempt to rebind the falsely bound variable
      when we codegen the vaiables declaration, which
      must occur before using the variable in a well
      formed application statement. anyways, for safety!
    */
    assert(argument_result.GetFirst() != nullptr);
    argument_values.push_back(argument_result.GetFirst());
  }

  auto *call = [&]() {
    if (argument_values.empty()) {
      return env.instruction_builder->CreateCall(
          llvm::FunctionCallee(function));
    }
    return env.instruction_builder->CreateCall(llvm::FunctionCallee(function),
                                               argument_values);
  }();

  call->setAttributes(function->getAttributes());
  result = call;
}

/*
  Okay, llvm::ConstantArray works great if the programmer
  only ever wants to create constant arrays, but what about
  implementing an array literal which composes the results
  of an array of expressions? well, then we need an
  array on the stack. that works, however it becomes
  inefficient in two circumstances, underneath a bind
  statement, and as temporary variables.
  The Bind statement introduces an inefficiency when the Bind Statement
  allocate's for the new local and copy's it's initializer.
  Temporary variables introduce an inefficiency in that if we
  were to implement them as full blown locals, since they only
  get use in very local contexts we end up wasting space on
  the stack leaving them allocated for the entire function
  lifetime.
  if an array literal appears in a larger expression, we
  have to allocate a temporary, and if that expression is
  a bind expression we have to avoid reallocating the space,
  and we are no longer allocating a temporary for the array.
  bind could very well receive a pointer to an allocation,
  (which implies this routine is smaller and simpler), but
  how does bind know not to allocate for an array type?
  right now bind simply always allocates and stores,
  and since Tuple and Array both construct constants
  bind doesn't duplicate the space requirements.

  we could implement temporaries if we kept track of
  all the space we allocated when compiling any given
  term, then in any given scope after compiling a single
  term, we emit a deallocate stack space instruction equal
  to the amount that the single Ast term allocated.
  this could be communicated by way of a variable in the
  environment relatively simply.
  1) how do we know it's a temporary allocation?
    A) use a flag [inBindExpression], because only objects
    constructed underneath a bind expression are truly
    local variables.
*/
void CodegenVisitor::Visit(const Array *array) const noexcept {
  /*
    it is a bit of a bummer that I cannot initialize
    this vector on construction, but given that each
    element has the potential to fail, I can't think
    of a way to refactor this into a lambda.
  */
  std::vector<llvm::Constant *> celements;
  celements.reserve(array->GetElements().size());

  auto cursor = celements.begin();
  for (const auto &element : *array) {
    auto element_result = Compute(element, this);
    if (!element_result) {
      result = element_result;
      return;
    }
    auto *element_value = element_result.GetFirst();

    auto *celement      = llvm::dyn_cast<llvm::Constant>(element_value);
    if (celement == nullptr) {
      std::string errmsg = "Array Initializer [";
      errmsg             += ToString(element);
      errmsg             += "] is not a Constant";
      result =
          Error(Error::Code::NonConstArrayInit, element->GetLocation(), errmsg);
      return;
    }

    celements.emplace_back(celement);
  }

  /*
    This assert enforces that Typecheck is called on
    every Ast before Codegen is ever called.
  */
  auto cached = array->GetCachedType();
  assert(cached.has_value());
  auto *cached_type = cached.value();

  auto *array_layout_type =
      llvm::cast<llvm::StructType>(ToLLVM(cached_type, env));

  auto *integer_type = llvm::cast<llvm::IntegerType>(
      array_layout_type->getTypeAtIndex((unsigned)0));
  auto *array_type = llvm::cast<llvm::ArrayType>(
      array_layout_type->getTypeAtIndex((unsigned)1));

  auto *constant_array = llvm::ConstantArray::get(array_type, celements);
  auto *array_size =
      llvm::ConstantInt::get(integer_type, array->GetElements().size());

  result = llvm::ConstantStruct::get(array_layout_type,
                                     {array_size, constant_array});
}

/*
  We lower and assignment expression to a store expression.
*/
void CodegenVisitor::Visit(const Assignment *assignment) const noexcept {
  env.flags->OnTheLHSOfAssignment(true);
  auto left_result = Compute(assignment->GetLeft(), this);
  env.flags->OnTheLHSOfAssignment(false);
  if (!left_result) {
    result = left_result;
    return;
  }
  auto *left_value  = left_result.GetFirst();

  auto right_result = Compute(assignment->GetRight(), this);
  if (!right_result) {
    result = right_result;
    return;
  }
  auto *right_value = right_result.GetFirst();

  StoreValue(left_value->getType(), left_value, right_value, env);
  result = right_value;
}

/*
  We lower bind expressions to an alloca plus a StoreValue.
*/
void CodegenVisitor::Visit(const Bind *bind) const noexcept {
  auto bound = env.bindings->LookupLocal(bind->GetSymbol());
  assert(!bound.has_value());

  auto affix_type_cache = bind->GetAffix()->GetCachedType();
  assert(affix_type_cache.has_value());
  auto *affix_type  = affix_type_cache.value();

  auto affix_result = Compute(bind->GetAffix(), this);
  if (!affix_result) {
    result = affix_result;
    return;
  }
  auto *affix_value = affix_result.GetFirst();

  auto *alloca = AllocateVariable(bind->GetSymbol(), affix_value->getType(),
                                  env, affix_value);

  /*
    We don't return the variable allocation as the result of
    a bind expression because in the case of a composition
    of bind expressions we don't want to set the initializer
    to the previous bind's allocation in the sequence.
    (though, it might not matter if we call StoreValue to
    initialize.)
  */
  env.bindings->Bind(bind->GetSymbol(), affix_type, alloca);
  result = affix_value;
}

/*
  We lower binop expressions into their corresponding
  operations: add, multiply, divide, etc.
*/
void CodegenVisitor::Visit(const Binop *binop) const noexcept {
  auto left_cache = binop->GetLeft()->GetCachedType();
  assert(left_cache.has_value());
  auto *left_type  = left_cache.value();

  auto right_cache = binop->GetRight()->GetCachedType();
  assert(right_cache.has_value());
  auto *right_type = right_cache.value();

  auto left_result = Compute(binop->GetLeft(), this);
  if (!left_result) {
    result = left_result;
    return;
  }
  auto *left_value  = left_result.GetFirst();

  auto right_result = Compute(binop->GetRight(), this);
  if (!right_result) {
    result = right_result;
    return;
  }
  auto *right_value = right_result.GetFirst();

  auto literal      = env.binops->Lookup(binop->GetOp());
  assert(literal.has_value());

  auto implementation = literal->second->Lookup(left_type, right_type);
  assert(implementation.has_value());
  result = implementation->second->generate(left_value->getType(), left_value,
                                            right_value->getType(), right_value,
                                            env);
}

/*
  We lower a block into whatever it's component expressions
  lower into, in the sequence they appear.
*/
void CodegenVisitor::Visit(const Block *block) const noexcept {
  for (const auto &expression : *block) {
    result = Compute(expression, this);
    if (!result) {
      return;
    }
  }
  // result by the behavior of the loop holds the
  // value of the last expression in the block.
}

/*
  We lower a boolean value into an integer of width 1 with
  the value of the boolean.
*/
void CodegenVisitor::Visit(const Boolean *boolean) const noexcept {
  result = env.instruction_builder->getInt1(boolean->GetValue());
}

/*
  We lower a conditional into a branch between two basic
  blocks, and a phi node which merges the two incoming
  values from the branches.
*/
void CodegenVisitor::Visit(const Conditional *conditional) const noexcept {
  auto test_result = Compute(conditional->GetTest(), this);
  if (!test_result) {
    result = test_result;
    return;
  }
  auto *test_value = test_result.GetFirst();

  auto *then_BB =
      llvm::BasicBlock::Create(*env.context, "then", env.current_function);
  auto *else_BB  = llvm::BasicBlock::Create(*env.context, "else");
  auto *merge_BB = llvm::BasicBlock::Create(*env.context, "merge");

  env.instruction_builder->CreateCondBr(test_value, then_BB, else_BB);

  env.instruction_builder->SetInsertPoint(then_BB);
  auto first_result = Compute(conditional->GetFirst(), this);
  if (!first_result) {
    result = first_result;
    return;
  }
  auto *first_value = first_result.GetFirst();
  env.instruction_builder->CreateBr(merge_BB);
  then_BB = env.instruction_builder->GetInsertBlock();

  env.current_function->insert(env.current_function->end(), else_BB);

  env.instruction_builder->SetInsertPoint(else_BB);
  auto second_result = Compute(conditional->GetSecond(), this);
  if (!second_result) {
    result = second_result;
    return;
  }
  auto *second_value = second_result.GetFirst();
  env.instruction_builder->CreateBr(merge_BB);
  else_BB = env.instruction_builder->GetInsertBlock();

  env.current_function->insert(env.current_function->end(), merge_BB);
  env.instruction_builder->SetInsertPoint(merge_BB);
  auto *phi =
      env.instruction_builder->CreatePHI(first_value->getType(), 2, "phi");
  phi->addIncoming(first_value, then_BB);
  phi->addIncoming(second_value, else_BB);
  result = phi;
}

/*
  We lower a dot expression into a GEP instruction along with a load.
*/
void CodegenVisitor::Visit(const Dot *dot) const noexcept {
  auto left_result = Compute(dot->GetLeft(), this);
  if (!left_result) {
    result = left_result;
    return;
  }
  auto *left_value = left_result.GetFirst();
  auto *index      = llvm::dyn_cast<Integer>(dot->GetRight().get());
  assert(index != nullptr);
  auto *struct_type = llvm::cast<llvm::StructType>(left_value->getType());
  auto *gep         = env.instruction_builder->CreateConstGEP2_32(
      struct_type, left_value, 0, index->GetValue());
  auto *element_type = struct_type->getTypeAtIndex(index->GetValue());
  result             = LoadValue(element_type, gep, env);
}

/*
  We lower a function into a llvm::Function.
*/
void CodegenVisitor::Visit(const Function *function) const noexcept {
  auto is_main = strcmp(function->GetName(), "main") == 0;

  auto cache   = function->GetCachedType();
  assert(cache.has_value());
  auto *cache_type         = cache.value();
  auto *pink_function_type = llvm::cast<FunctionType>(cache_type);

  auto *llvm_return_type   = ToLLVM(pink_function_type->GetReturnType(), env);
  auto *llvm_function_type = [&]() {
    if (is_main) {
      auto *main_function_type = env.types->GetFunctionType(
          env.types->GetVoidType(), pink_function_type->GetArguments());
      return llvm::cast<llvm::FunctionType>(ToLLVM(main_function_type, env));
    }
    return llvm::cast<llvm::FunctionType>(ToLLVM(pink_function_type, env));
  }();

  auto *llvm_function = llvm::Function::Create(
      llvm_function_type, llvm::Function::ExternalLinkage, function->GetName(),
      *env.llvm_module);

  /*
    if this function returns a structure then we
    have already promoted it's return type to void,
    and added a parameter at the beginning of the
    argument list. This argument needs to be given
    the sret<Ty> attribute to communicate to llvm
    that this struct is the return value.
    this also means that the size of the argument list
    is off by one compared to the function type as in
    the source text. so we also have to start counting
    the index from 1.
    if the function returns a singleValueType the return
    value simply sits in the return register and we can
    start counting arguments from 0.
  */
  std::size_t index = [&]() {
    if (llvm_return_type->isSingleValueType()) {
      llvm::AttrBuilder return_attribute(*env.context);
      return_attribute.addStructRetAttr(llvm_return_type);
      llvm_function->addParamAttrs((unsigned)0, return_attribute);
      return 1;
    }
    return 0;
  }();

  const auto &pink_arguments = pink_function_type->GetArguments();
  for (; index < llvm_function_type->getNumParams(); index++) {
    llvm::AttrBuilder parameter_attribute(*env.context);
    auto             *parameter_type = ToLLVM(pink_arguments[index], env);

    if (!parameter_type->isSingleValueType()) {
      parameter_attribute.addByValAttr(parameter_type);
      llvm_function->addParamAttrs(index, parameter_attribute);
    }
  }

  auto *entry_BB = llvm::BasicBlock::Create(
      *env.context, function->GetName() + std::string("_entry"), llvm_function);
  auto entry_point = entry_BB->getFirstInsertionPt();

  auto local_builder =
      std::make_shared<llvm::IRBuilder<>>(entry_BB, entry_point);
  auto local_env = Environment::NewLocalEnv(env, function->GetScope(),
                                            local_builder, llvm_function);

  // Set up the arguments within the function's first BasicBlock,
  // A) Allocate all arguments
  // B) Initialize all arguments
  // we must do A for all arguments before we can do B for any
  // argument, because the llvm optimizer handles all allocas
  // at the beginning of the functions first BasicBlock
  std::vector<llvm::AllocaInst *> arg_allocas;
  arg_allocas.reserve(function->GetArguments().size());
  auto allocate_argument = [&](const llvm::Argument &arg) {
    arg_allocas.emplace_back(env.instruction_builder->CreateAlloca(
        arg.getType(), env.data_layout.getAllocaAddrSpace(), nullptr,
        arg.getName()));
  };
  std::for_each(llvm_function->arg_begin(), llvm_function->arg_end(),
                allocate_argument);

  auto  alloc_cursor    = arg_allocas.begin();
  auto  alloc_end       = arg_allocas.end();
  auto *llvm_arg_cursor = llvm_function->arg_begin();
  auto *llvm_arg_end    = llvm_function->arg_end();
  auto  pink_arg_cursor = function->GetArguments().begin();
  auto  pink_arg_end    = function->GetArguments().end();
  while ((alloc_cursor != alloc_end) && (llvm_arg_cursor != llvm_arg_end) &&
         (pink_arg_cursor != pink_arg_end)) {
    auto       *alloc    = *alloc_cursor;
    auto       &llvm_arg = *llvm_arg_cursor;
    const auto &pink_arg = *pink_arg_cursor;

    StoreValue(llvm_arg.getType(), alloc, &llvm_arg, env);
    env.bindings->Bind(pink_arg.first, pink_arg.second, alloc);

    alloc_cursor++;
    llvm_arg_cursor++;
    pink_arg_cursor++;
  }
  // this is unfortunate code, the design of
  // Environment needs updating. specifically
  // removing the free-floating SymbolTable::Pointers
  // from the Ast, and implementing a Stack of Scopes
  // then pushing/poping a scope can be a method of
  // the environment. much cleaner!
  Environment &old_env = env;
  env                  = *local_env;
  auto body_result     = Compute(function->GetBody(), this);
  if (!body_result) {
    result = body_result;
    return;
  }
  auto *body_value = body_result.GetFirst();
  env              = old_env;

  if (is_main) {
    CodegenSysExit(body_value, env);
    env.instruction_builder->CreateRetVoid();
  } else {
    if (llvm_return_type->isSingleValueType()) {
      local_builder->CreateRet(body_value);
    } else {
      StoreValue(llvm_return_type, llvm_function->getArg(0), body_value,
                 *local_env);
    }
  }

  std::string              buffer;
  llvm::raw_string_ostream out(buffer);
  if (llvm::verifyFunction(*llvm_function, &out)) {
    FatalError(buffer, __FILE__, __LINE__);
  }

  env.bindings->Bind(function->GetName(), pink_function_type, llvm_function);
  result = llvm_function;
}

void CodegenVisitor::Visit(const Integer *integer) const noexcept {
  result = env.instruction_builder->getInt32(integer->GetValue());
}

void CodegenVisitor::Visit(const Nil *nil) const noexcept {
  result = env.instruction_builder->getInt1(false);
}

void CodegenVisitor::Visit(const Subscript *subscript) const noexcept {
  auto left_result = Compute(subscript->GetLeft(), this);
  if (!left_result) {
    result = left_result;
    return;
  }
  auto *left_value = left_result.GetFirst();

  auto left_cache  = subscript->GetLeft()->GetCachedType();
  assert(left_cache.has_value());
  auto *left_type   = left_cache.value();

  auto right_result = [&]() {
    CodegenResult res;
    // this seems fine for now, and works. But it feels
    // like this pattern could become hard to understand
    // and maintain if we have to keep adding flags and
    // checks like these for each place it matters. as
    // it is both hard-coded and necessary
    // in multiple locations. maybe some kind of flags
    // bitset that was available in each Ast node,
    // it could keep track of const, ownership, others,.
    // wait, that might not be apropo of this particular
    // problem, as the Ast isn't necessarily concerned
    // with what is above it in the Ast? this flag is
    // only used in the code generation visitor.
    if (env.flags->OnTheLHSOfAssignment()) {
      env.flags->OnTheLHSOfAssignment(false);
      res = Compute(subscript->GetRight(), this);
      env.flags->OnTheLHSOfAssignment(true);
    } else {
      res = Compute(subscript->GetRight(), this);
    }
    return res;
  }();
  if (!right_result) {
    result = right_result;
    return;
  }
  auto *right_value = right_result.GetFirst();

  if (auto *array_type = llvm::dyn_cast<ArrayType>(left_type);
      array_type != nullptr) {
    auto *llvm_array_type =
        llvm::cast<llvm::StructType>(ToLLVM(array_type, env));
    auto *llvm_element_type = ToLLVM(array_type->GetElementType(), env);
    result = ArraySubscript(llvm_array_type, llvm_element_type, left_value,
                            right_value, env);
    return;
  }

  if (auto *slice_type = llvm::dyn_cast<SliceType>(left_type);
      slice_type != nullptr) {
    auto *llvm_slice_type =
        llvm::cast<llvm::StructType>(ToLLVM(slice_type, env));
    auto *llvm_element_type = ToLLVM(slice_type->GetPointeeType(), env);
    result = SliceSubscript(llvm_slice_type, llvm_element_type, left_value,
                            right_value, env);
    return;
  }

  // we should never reach here, but just in case
  assert(false);
}

void CodegenVisitor::Visit(const Tuple *tuple) const noexcept {
  auto tuple_result = tuple->GetCachedType();
  assert(tuple_result.has_value());
  auto *tuple_type  = tuple_result.value();

  auto *struct_type = llvm::cast<llvm::StructType>(ToLLVM(tuple_type, env));

  std::vector<llvm::Constant *> celements;
  celements.reserve(tuple->GetElements().size());

  auto tuple_cursor     = tuple->begin();
  auto tuple_end        = tuple->end();
  auto celements_cursor = celements.begin();
  auto celements_end    = celements.end();
  while ((tuple_cursor != tuple_end) && (celements_cursor != celements_end)) {
    auto element_result = Compute(*tuple_cursor, this);
    if (!element_result) {
      result = element_result;
      return;
    }
    auto *element_value = element_result.GetFirst();
    auto *initalizer    = llvm::dyn_cast<llvm::Constant>(element_value);
    assert(initalizer != nullptr);
    celements.emplace_back(initalizer);

    tuple_cursor++;
    celements_cursor++;
  }

  result = llvm::ConstantStruct::get(struct_type, celements);
}

static auto CodegenUnopAddressOf(const Unop *unop, Environment &env,
                                 const CodegenVisitor *visitor)
    -> CodegenResult {
  if (env.flags->OnTheLHSOfAssignment()) {
    return Error(Error::Code::ValueCannotBeAssigned, unop->GetLocation());
  }

  env.flags->WithinAddressOf(true);
  auto right_result = visitor->Compute(unop->GetRight(), visitor);
  env.flags->WithinAddressOf(false);
  return right_result;
};

static auto CodegenUnopDereferencePointer(const Unop *unop, Type *right_type,
                                          Environment          &env,
                                          const CodegenVisitor *visitor)
    -> CodegenResult {
  // if we are dereferencing on the left of an assignment
  // expression, we want to suppress a single load instruction,
  // so we pretend we aren't under a dereference operation
  if (env.flags->OnTheLHSOfAssignment()) {
    return visitor->Compute(unop->GetRight(), visitor);
  }

  env.flags->WithinDereferencePtr(true);
  auto right_result = visitor->Compute(unop->GetRight(), visitor);
  env.flags->WithinDereferencePtr(false);
  if (!right_result) {
    return right_result;
  }
  auto *right_value       = right_result.GetFirst();

  // we know this is a pointer type, because we can
  // only validly typecheck a dereference operation
  // on a PointerType
  auto *pointer_type      = llvm::cast<pink::PointerType>(right_type);
  auto *llvm_pointee_type = ToLLVM(pointer_type, env);
  // Note: this is the load for the dereference operation,
  // the generator expression is a no-op for address of and dereference.
  return LoadValue(llvm_pointee_type, right_value, env);
};

void CodegenVisitor::Visit(const Unop *unop) const noexcept {
  auto right_cache = unop->GetRight()->GetCachedType();
  assert(right_cache.has_value());
  auto *right_type  = right_cache.value();

  auto right_result = [&]() -> CodegenResult {
    if (strcmp(unop->GetOp(), "&") == 0) {
      return CodegenUnopAddressOf(unop, env, this);
    }

    if (strcmp(unop->GetOp(), "*") == 0) {
      return CodegenUnopDereferencePointer(unop, right_type, env, this);
    }

    return Compute(unop->GetRight(), this);
  }();
  auto *right_value = right_result.GetFirst();

  auto literal      = env.unops->Lookup(unop->GetOp());
  assert(literal.has_value());

  auto implementation = literal->second->Lookup(right_type);
  assert(implementation.has_value());

  result = implementation->second->generate(right_value, env);
}

void CodegenVisitor::Visit(const Variable *variable) const noexcept {
  auto bound = env.bindings->Lookup(variable->GetSymbol());
}

void CodegenVisitor::Visit(const While *loop) const noexcept {}

[[nodiscard]] auto Codegen(const Ast::Pointer &ast, Environment &env) noexcept
    -> CodegenResult {
  // 1/27/2023:
  // technically this constructor does extra work, (not too much)
  // as internally we construct a VisitorResult with a member to
  // store the result, however this particular instance of the visitor
  // is not ever going to have a return value written to it because
  // it is only ever used to pass in the extra argument members.
  // I cannot figure out a way to remove this extra construction.
  // As I need to pass a CodegenVisitor * to VisitorResult::Compute
  // in order for the new visitor it constructs to get a copy of any
  // members which this visitor has This is necessary because the visitor
  // A) needs more parameters than the one which *::Accept and *::Visit
  //    allows. (namely the single pointer to the derived class)
  // B) needs to allocate another object to hold another return value.
  // To pass in extra parameters we can use more visitor members, and
  // it means we need a valid Visitor to pass to Compute for the first
  // call, which means we cannot factor the Constructor out of this function
  // either, which means that this constructor allocates an extra return
  // value slot, which is never accessable within a call to *::Visit,
  // because inside Compute we have to allocate a new visitor every time.
  // (I used *::Visit, because this is an issue with every visitor that
  //  needs to return a value via the VisitorResult pattern; which
  //  is every visitor in the project! But how else can we get a Polymorphic
  //  return slot for each individual visitor?)
  CodegenVisitor visitor(env);
  return visitor.Compute(ast, &visitor);
}
} // namespace pink
