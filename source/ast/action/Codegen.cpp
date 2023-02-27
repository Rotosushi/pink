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

#include "runtime/AllocateVariable.h"
#include "runtime/ArraySubscript.h"
#include "runtime/LoadValue.h"
#include "runtime/SliceSubscript.h"
#include "runtime/StoreValue.h"
#include "runtime/sys/SysExit.h"

#include "llvm/IR/Verifier.h"

namespace pink {
class CodegenVisitor : public ConstVisitorResult<CodegenVisitor,
                                                 const Ast::Pointer &,
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
  void Visit(const IfThenElse *conditional) const noexcept override;
  void Visit(const Dot *dot) const noexcept override;
  void Visit(const Function *function) const noexcept override;
  void Visit(const Integer *integer) const noexcept override;
  void Visit(const Nil *nil) const noexcept override;
  void Visit(const Subscript *subscript) const noexcept override;
  void Visit(const Tuple *tuple) const noexcept override;
  void Visit(const Unop *unop) const noexcept override;
  void Visit(const Variable *variable) const noexcept override;
  void Visit(const While *loop) const noexcept override;

  CodegenVisitor(Environment &env) noexcept
      : ConstVisitorResult(),
        env(env) {}
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
  auto *callee_value = Compute(application->GetCallee(), this);
  assert(callee_value != nullptr);

  auto *function = llvm::dyn_cast<llvm::Function>(callee_value);
  assert(function != nullptr);

  std::vector<llvm::Value *> argument_values;

  for (const auto &argument : *application) {
    auto *argument_value = Compute(argument, this);
    assert(argument_value != nullptr);
    argument_values.push_back(argument_value);
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

*/
void CodegenVisitor::Visit(const Array *array) const noexcept {
  const auto *cached_type = array->GetCachedTypeOrAssert();

  auto *array_layout_type =
      llvm::cast<llvm::StructType>(ToLLVM(cached_type, env));

  auto *array_alloca = env.instruction_builder->CreateAlloca(array_layout_type);

  auto *array_size =
      env.instruction_builder->getInt64(array->GetElements().size());
  auto *array_size_type =
      llvm::cast<llvm::IntegerType>(array_layout_type->getTypeAtIndex(0U));
  auto *array_elements_type =
      llvm::cast<llvm::ArrayType>(array_layout_type->getTypeAtIndex(1U));
  auto *element_type = array_elements_type->getArrayElementType();

  auto *array_size_pointer =
      env.instruction_builder->CreateConstInBoundsGEP2_32(array_layout_type,
                                                          array_alloca,
                                                          0,
                                                          0,
                                                          "array_size");

  auto *array_elements_pointer =
      env.instruction_builder->CreateConstInBoundsGEP2_32(array_layout_type,
                                                          array_alloca,
                                                          0,
                                                          0,
                                                          "array_elements");

  unsigned index = 0;
  for (const auto &element : *array) {
    auto *element_value = Compute(element, this);
    assert(element_value != nullptr);
    auto *element_pointer = env.instruction_builder->CreateConstInBoundsGEP2_32(
        array_elements_type,
        array_elements_pointer,
        0,
        index);
    StoreValue(element_type, element_pointer, element_value, env);
    index += 1;
  }

  StoreValue(array_size_type, array_size_pointer, array_size, env);

  result = array_alloca;
}

/*
  We lower and assignment expression to a store expression.
*/
void CodegenVisitor::Visit(const Assignment *assignment) const noexcept {
  env.OnTheLHSOfAssignment(true);
  auto *left_value = Compute(assignment->GetLeft(), this);
  env.OnTheLHSOfAssignment(false);
  assert(left_value != nullptr);

  auto *right_value = Compute(assignment->GetRight(), this);
  assert(right_value != nullptr);

  StoreValue(left_value->getType(), left_value, right_value, env);
  result = right_value;
}

/*
  We lower bind expressions to an alloca plus a StoreValue.
*/
void CodegenVisitor::Visit(const Bind *bind) const noexcept {
  // this check was done in typechecking, it isn't truly
  // 'necessary' it is here more for code correctness.
  auto found = env.LookupLocalVariable(bind->GetSymbol());
  assert(!found);

  const auto *affix_type = bind->GetAffix()->GetCachedTypeOrAssert();

  env.WithinBindExpression(true);
  auto *affix_value = Compute(bind->GetAffix(), this);
  assert(affix_value != nullptr);
  env.WithinBindExpression(false);

  // #RULE we must allocate stack space for singleValueType()s
  // #RULE We must not allocate space for non-singleValueType()s
  // as the Codegen visitor for non-singleValueType() literals
  // allocates stack space for them.
  auto *llvm_type = ToLLVM(affix_type, env);
  if (llvm_type->isSingleValueType()) {
    affix_value =
        AllocateVariable(bind->GetSymbol(), llvm_type, env, affix_value);
  }

  env.BindVariable(bind->GetSymbol(), affix_type, affix_value);
  result = affix_value;
}

/*
  We lower binop expressions into their corresponding
  operations: add, multiply, divide, etc.
*/
void CodegenVisitor::Visit(const Binop *binop) const noexcept {
  const auto *left_type      = binop->GetLeft()->GetCachedTypeOrAssert();
  auto       *llvm_left_type = ToLLVM(left_type, env);
  assert(llvm_left_type != nullptr);

  const auto *right_type      = binop->GetRight()->GetCachedTypeOrAssert();
  auto       *llvm_right_type = ToLLVM(right_type, env);
  assert(llvm_right_type != nullptr);

  auto *left_value = Compute(binop->GetLeft(), this);
  assert(left_value != nullptr);

  auto *right_value = Compute(binop->GetRight(), this);
  assert(right_value != nullptr);

  auto optional_literal = env.LookupBinop(binop->GetOp());
  assert(optional_literal.has_value());
  auto literal = optional_literal.value();

  auto optional_implementation = literal.Lookup(left_type, right_type);
  assert(optional_implementation.has_value());
  auto implementation = optional_implementation.value();

  result = implementation.Generate()(llvm_left_type,
                                     left_value,
                                     llvm_right_type,
                                     right_value,
                                     env);
}

/*
  We lower a block into whatever it's component expressions
  lower into, in the sequence they appear.
*/
void CodegenVisitor::Visit(const Block *block) const noexcept {
  for (const auto &expression : *block) {
    result = Compute(expression, this);
    assert(result != nullptr);
  }
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

  #TODO: add if blocks without else branches. (these have Type Nil)
*/
void CodegenVisitor::Visit(const IfThenElse *conditional) const noexcept {
  assert(env.current_function != nullptr);
  auto *test_value = Compute(conditional->GetTest(), this);
  assert(test_value != nullptr);

  // #NOTE: 2/25/2023: We must not add the 'else' and 'merge'
  // basic blocks to the function upon creation, as the Codegen
  // for the first or second alternative may change which
  // basic block is last in the function, and we want to append
  // exclusively to the end of the functions basic block list.
  auto *then_BB =
      llvm::BasicBlock::Create(*env.context, "then", env.current_function);
  auto *else_BB  = llvm::BasicBlock::Create(*env.context, "else");
  auto *merge_BB = llvm::BasicBlock::Create(*env.context, "merge");

  env.instruction_builder->CreateCondBr(test_value, then_BB, else_BB);
  env.instruction_builder->SetInsertPoint(then_BB);

  auto *first_value = Compute(conditional->GetFirst(), this);
  assert(first_value != nullptr);

  env.instruction_builder->CreateBr(merge_BB);
  then_BB = env.instruction_builder->GetInsertBlock();

  env.current_function->insert(env.current_function->end(), else_BB);
  env.instruction_builder->SetInsertPoint(else_BB);

  auto *second_value = Compute(conditional->GetSecond(), this);
  assert(second_value != nullptr);

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
  auto *left_value = Compute(dot->GetLeft(), this);
  assert(left_value != nullptr);

  auto *index = llvm::dyn_cast<Integer>(dot->GetRight().get());
  assert(index != nullptr);
  auto *struct_type = llvm::cast<llvm::StructType>(left_value->getType());
  auto *gep         = env.instruction_builder->CreateConstGEP2_32(
      struct_type,
      left_value,
      0,
      static_cast<unsigned>(index->GetValue()));
  auto *element_type =
      struct_type->getTypeAtIndex(static_cast<unsigned>(index->GetValue()));
  result = LoadValue(element_type, gep, env);
}

/*
  We lower a function into a llvm::Function.
*/
void CodegenVisitor::Visit(const Function *function) const noexcept {
  auto is_main = strcmp(function->GetName(), "main") == 0;

  const auto *cache_type         = function->GetCachedTypeOrAssert();
  const auto *pink_function_type = llvm::cast<FunctionType>(cache_type);

  std::cout << "pink type [" << ToString(pink_function_type) << "]\n";

  auto *llvm_return_type   = ToLLVM(pink_function_type->GetReturnType(), env);
  auto *llvm_function_type = [&]() {
    if (is_main) {
      const auto *main_function_type =
          env.GetFunctionType(env.GetVoidType(),
                              pink_function_type->GetArguments());
      return llvm::cast<llvm::FunctionType>(ToLLVM(main_function_type, env));
    }
    return llvm::cast<llvm::FunctionType>(ToLLVM(pink_function_type, env));
  }();

  auto *llvm_function = llvm::Function::Create(llvm_function_type,
                                               llvm::Function::ExternalLinkage,
                                               function->GetName(),
                                               *env.module);

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
    the index from 1. additionally, any non-SingleValueType
    will have been converted to an opaque pointer type,
    so we must use the arguments converted from the
    pink::FunctionType one by one.
    #NOTE: I would prefer to add parameter attributes
    when we convert the FunctionType within ToLLVM(),
    however the attributes themselves are associated
    with each function, as llvm::FunctionTypes may
    be shared among llvm::Functions. This means we
    are forced to walk the list of arguments types
    twice.
    if the function returns a singleValueType the return
    value simply sits in the return register and we can
    start counting arguments from 0.
  */
  unsigned index = [&]() {
    if (llvm_return_type->isSingleValueType() &&
        !llvm_return_type->isVoidTy()) {
      llvm::AttrBuilder return_attribute(*env.context);
      return_attribute.addStructRetAttr(llvm_return_type);
      llvm_function->addParamAttrs(0U, return_attribute);
      return 1U;
    }
    return 0U;
  }();

  const auto &arguments            = pink_function_type->GetArguments();
  const auto  number_of_parameters = llvm_function_type->getNumParams();
  for (; index < number_of_parameters; ++index) {
    llvm::AttrBuilder parameter_attribute(*env.context);
    auto             *parameter_type = ToLLVM(arguments[index], env);

    if (!parameter_type->isSingleValueType() && !parameter_type->isVoidTy()) {
      parameter_attribute.addByValAttr(parameter_type);
      llvm_function->addParamAttrs(index, parameter_attribute);
    }
  }

  auto *entry_BB =
      llvm::BasicBlock::Create(*env.context,
                               function->GetName() + std::string("entry"),
                               llvm_function);

  env.instruction_builder->SetInsertPoint(entry_BB);
  env.PushScope();

  // Set up the arguments within the function's first BasicBlock,
  // A) Allocate all arguments
  // B) Initialize all arguments
  // we must do A for all arguments before we can do B for any
  // argument, because the llvm optimizer best handles all allocas
  // at the beginning of the functions first BasicBlock
  std::vector<llvm::AllocaInst *> arg_allocas;
  arg_allocas.reserve(function->GetArguments().size());
  auto allocate_argument = [&](const llvm::Argument &arg) {
    arg_allocas.emplace_back(env.instruction_builder->CreateAlloca(
        arg.getType(),
        env.module->getDataLayout().getAllocaAddrSpace(),
        nullptr,
        arg.getName()));
  };
  std::for_each(llvm_function->arg_begin(),
                llvm_function->arg_end(),
                allocate_argument);

  auto alloc_cursor    = arg_allocas.begin();
  auto alloc_end       = arg_allocas.end();
  auto pink_arg_cursor = function->GetArguments().begin();
  auto pink_arg_end    = function->GetArguments().end();
  for (auto &llvm_arg : llvm_function->args()) {
    auto       *alloc    = *alloc_cursor;
    const auto &pink_arg = *pink_arg_cursor;

    StoreValue(llvm_arg.getType(), alloc, &llvm_arg, env);
    env.BindVariable(pink_arg.first, pink_arg.second, alloc);

    alloc_cursor++;
    pink_arg_cursor++;
    if (alloc_cursor == alloc_end || pink_arg_cursor == pink_arg_end) {
      break;
    }
  }

  auto *body_value = Compute(function->GetBody(), this);
  assert(body_value != nullptr);

  if (is_main) {
    SysExit(body_value, env);
    env.instruction_builder->CreateRetVoid();
  } else {
    if (llvm_return_type->isSingleValueType()) {
      env.instruction_builder->CreateRet(body_value);
    } else {
      StoreValue(llvm_return_type, llvm_function->getArg(0), body_value, env);
    }
  }

  std::cerr << "number of function attributes ["
            << llvm_function->getAttributes().getNumAttrSets() << "\n]";

  std::string              buffer;
  llvm::raw_string_ostream out(buffer);
  if (llvm::verifyFunction(*llvm_function, &out)) {
    std::cerr << "llvm function [\n"
              << LLVMValueToString(llvm_function) << "]\n"
              << "llvm type [" << LLVMTypeToString(llvm_function_type) << "]\n";
    FatalError(buffer, __FILE__, __LINE__);
  }
  // We just emitted a function, so we know we
  // are about to enter global scope, and we cannot
  // emit instructions into global scope, so we must
  // clear the insertion point.
  env.instruction_builder->ClearInsertionPoint();
  env.BindVariable(function->GetName(), pink_function_type, llvm_function);
  result = llvm_function;
}

void CodegenVisitor::Visit(const Integer *integer) const noexcept {
  result = env.instruction_builder->getInt64(integer->GetValue());
}

void CodegenVisitor::Visit(const Nil *nil) const noexcept {
  assert(nil != nullptr);
  result = env.instruction_builder->getInt1(false);
}

void CodegenVisitor::Visit(const Subscript *subscript) const noexcept {
  auto *left_value = Compute(subscript->GetLeft(), this);
  assert(left_value != nullptr);

  auto left_cache = subscript->GetLeft()->GetCachedType();
  assert(left_cache.has_value());
  const auto *left_type = left_cache.value();

  auto *right_value = [&]() {
    CodegenResult res = nullptr;
    if (env.OnTheLHSOfAssignment()) {
      env.OnTheLHSOfAssignment(false);
      res = Compute(subscript->GetRight(), this);
      env.OnTheLHSOfAssignment(true);
    } else {
      res = Compute(subscript->GetRight(), this);
    }
    return res;
  }();

  if (const auto *array_type = llvm::dyn_cast<ArrayType const>(left_type);
      array_type != nullptr) {
    auto *llvm_array_type =
        llvm::cast<llvm::StructType>(ToLLVM(array_type, env));
    auto *llvm_element_type = ToLLVM(array_type->GetElementType(), env);
    result                  = ArraySubscript(llvm_array_type,
                            llvm_element_type,
                            left_value,
                            right_value,
                            env);
    return;
  }

  if (const auto *slice_type = llvm::dyn_cast<SliceType const>(left_type);
      slice_type != nullptr) {
    auto *llvm_slice_type =
        llvm::cast<llvm::StructType>(ToLLVM(slice_type, env));
    auto *llvm_element_type = ToLLVM(slice_type->GetPointeeType(), env);
    result                  = SliceSubscript(llvm_slice_type,
                            llvm_element_type,
                            left_value,
                            right_value,
                            env);
    return;
  }

  // we should never reach here, but just in case
  assert(false);
}

void CodegenVisitor::Visit(const Tuple *tuple) const noexcept {
  auto tuple_result = tuple->GetCachedType();
  assert(tuple_result.has_value());
  const auto *tuple_type = tuple_result.value();

  auto *tuple_layout_type =
      llvm::cast<llvm::StructType>(ToLLVM(tuple_type, env));
  auto *tuple_alloca = env.instruction_builder->CreateAlloca(tuple_layout_type);

  unsigned index = 0;
  for (const auto &element : *tuple) {
    auto *element_value = Compute(element, this);
    assert(element_value != nullptr);
    auto *element_type = tuple_layout_type->getStructElementType(index);
    auto *element_pointer =
        env.instruction_builder->CreateConstInBoundsGEP2_32(tuple_layout_type,
                                                            tuple_alloca,
                                                            0,
                                                            index);
    StoreValue(element_type, element_pointer, element_value, env);
  }

  result = tuple_alloca;
}

static auto CodegenUnopAddressOf(const Unop           *unop,
                                 Environment          &env,
                                 const CodegenVisitor *visitor)
    -> CodegenResult {
  env.WithinAddressOf(true);
  auto *right_result = visitor->Compute(unop->GetRight(), visitor);
  env.WithinAddressOf(false);
  assert(right_result != nullptr);
  return right_result;
}

static auto CodegenUnopDereferencePointer(const Unop           *unop,
                                          Type::Pointer         right_type,
                                          Environment          &env,
                                          const CodegenVisitor *visitor)
    -> CodegenResult {
  // if we are dereferencing on the left of an assignment
  // expression, we want to suppress a single load instruction,
  // so we pretend we aren't under a dereference operation
  if (env.OnTheLHSOfAssignment()) {
    return visitor->Compute(unop->GetRight(), visitor);
  }

  env.WithinDereferencePtr(true);
  auto *right_value = visitor->Compute(unop->GetRight(), visitor);
  env.WithinDereferencePtr(false);
  assert(right_value != nullptr);

  // we know this is a pointer type, because we can
  // only validly typecheck a dereference operation
  // on a PointerType
  const auto *pointer_type      = llvm::cast<pink::PointerType>(right_type);
  auto       *llvm_pointee_type = ToLLVM(pointer_type, env);
  // Note: this is the load for the dereference operation,
  // the generator expression is a no-op for address of and dereference.
  return LoadValue(llvm_pointee_type, right_value, env);
}

void CodegenVisitor::Visit(const Unop *unop) const noexcept {
  auto right_cache = unop->GetRight()->GetCachedType();
  assert(right_cache.has_value());
  const auto *right_type = right_cache.value();

  auto *right_value = [&]() -> CodegenResult {
    if (strcmp(unop->GetOp(), "&") == 0) {
      return CodegenUnopAddressOf(unop, env, this);
    }

    if (strcmp(unop->GetOp(), "*") == 0) {
      return CodegenUnopDereferencePointer(unop, right_type, env, this);
    }

    return Compute(unop->GetRight(), this);
  }();
  assert(right_value != nullptr);

  auto optional_literal = env.LookupUnop(unop->GetOp());
  assert(optional_literal.has_value());
  auto literal = optional_literal.value();

  auto optional_implementation = literal.Lookup(right_type);
  assert(optional_implementation.has_value());
  auto &implementation = optional_implementation.value();

  result = implementation.Generate()(right_value, env);
}

/*
  We lower a variable into whatever llvm::Value it is bound to.
*/
void CodegenVisitor::Visit(const Variable *variable) const noexcept {
  auto bound = env.LookupVariable(variable->GetSymbol());
  assert(bound.has_value());
  assert(bound->Value() != nullptr);
  result = LoadValue(ToLLVM(bound->Type(), env), bound->Value(), env);
}

/*
  We lower a while loop into three basic blocks, one
  for the test expression, one for the loop body, and
  one for after the loop is completed.
*/
void CodegenVisitor::Visit(const While *loop) const noexcept {
  assert(env.current_function != nullptr);
  auto *test_BB =
      llvm::BasicBlock::Create(*env.context, "test", env.current_function);
  auto *body_BB = llvm::BasicBlock::Create(*env.context, "body");
  auto *end_BB  = llvm::BasicBlock::Create(*env.context, "end");

  env.instruction_builder->CreateBr(test_BB);

  env.instruction_builder->SetInsertPoint(test_BB);
  auto *test_value = Compute(loop->GetTest(), this);
  assert(test_value != nullptr);
  env.instruction_builder->CreateCondBr(test_value, body_BB, end_BB);

  env.instruction_builder->SetInsertPoint(body_BB);
  env.current_function->insert(env.current_function->end(), body_BB);
  auto *body_value = Compute(loop->GetBody(), this);
  assert(body_value != nullptr);
  env.instruction_builder->CreateBr(test_BB);

  env.instruction_builder->SetInsertPoint(end_BB);
  env.current_function->insert(env.current_function->end(), end_BB);

  result = env.instruction_builder->getFalse();
}

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
  // members which this visitor has.
  // This is necessary because the visitor
  // A) needs more parameters than the one which *::Accept and *::Visit
  //    allows. (namely the single pointer to the derived class)
  // B) needs to allocate another object to hold another return value
  // on each call to Compute.
  CodegenVisitor visitor(env);
  return visitor.Compute(ast, &visitor);
}
} // namespace pink
