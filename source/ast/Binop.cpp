#include "ast/Binop.h"

#include "aux/Environment.h"

#include "type/SliceType.h"

#include "visitor/AstVisitor.h"

#include "kernel/AllocateVariable.h"
#include "kernel/Gensym.h"
#include "kernel/StoreAggregate.h"

namespace pink {
Binop::Binop(const Location &location, InternedString opr,
             std::unique_ptr<Ast> left, std::unique_ptr<Ast> right)
    : Ast(Ast::Kind::Binop, location), op(opr), left(std::move(left)),
      right(std::move(right)) {}

void Binop::Accept(AstVisitor *visitor) const { visitor->Visit(this); }

auto Binop::classof(const Ast *ast) -> bool {
  return ast->GetKind() == Ast::Kind::Binop;
}

auto Binop::ToString() const -> std::string {
  std::string result;
  if (llvm::isa<Binop>(left)) {
    result += "(" + left->ToString() + ")";
  } else {
    result += left->ToString();
  }

  result += " " + std::string(op) + " ";

  if (llvm::isa<Binop>(right)) {
    result += "(" + right->ToString() + ")";
  } else {
    result += right->ToString();
  }

  return result;
}

auto Binop::TypecheckLHSArrayAdd(ArrayType *array_type, Type *rhs_type,
                                 const Environment &env) const
    -> Outcome<Type *, Error> {
  auto *integer_type = llvm::dyn_cast<IntType>(rhs_type);
  if (integer_type == nullptr) {
    std::string errmsg =
        " Computing an offset into an array requires type Int, rhs has type: " +
        rhs_type->ToString();
    return {Error(Error::Code::ArgTypeMismatch, GetLoc(), errmsg)};
  }

  // since we cannot know the size until we Codegen the RHS
  // we emit a "runtime" slice, and we do no bounds checking.
  return {env.types->GetSliceType(array_type->member_type)};
}

auto Binop::TypecheckRHSArrayAdd(ArrayType *array_type, Type *lhs_type,
                                 const Environment &env) const
    -> Outcome<Type *, Error> {
  auto *integer_type = llvm::dyn_cast<IntType>(lhs_type);
  if (integer_type == nullptr) {
    std::string errmsg =
        " Computing an offset into an array requires type Int, lhs has type: " +
        lhs_type->ToString();
    return {Error(Error::Code::ArgTypeMismatch, GetLoc(), errmsg)};
  }
  return {env.types->GetSliceType(array_type->member_type)};
}

// compute the gep to the n'th pointer into the array,
// return a slice to that element. (pointing to the
// 'rest' of the array)
// \todo these functions don't belong in Binop.
auto Binop::CodegenGlobalArrayAdd(llvm::StructType *array_slice_type,
                                  llvm::Value *array_slice_ptr,
                                  llvm::ConstantInt *index_ptr,
                                  const Environment &env) const
    -> Outcome<llvm::Value *, Error> {
  assert(array_slice_ptr != nullptr);
  assert(array_slice_type != nullptr);
  assert(index_ptr != nullptr);
  assert(env.current_function == nullptr);
  assert(llvm::cast<llvm::GlobalVariable>(array_slice_ptr) != nullptr);

  auto *slice_size_type = env.instruction_builder->getInt64Ty();
  auto *slice_ptr_type = llvm::PointerType::get(
      *env.context, env.data_layout.getDefaultGlobalsAddressSpace());
  auto *slice_type =
      llvm::StructType::get(*env.context, {slice_size_type, slice_ptr_type});

  auto *slice_index_one_type = array_slice_type->getTypeAtIndex(1);
  auto *array_type = llvm::cast<llvm::ArrayType>(slice_index_one_type);

  // #RULE arrays have a size property
  // #RULE all array access must be bounds checked
  size_t idx = index_ptr->getLimitedValue();
  size_t size = array_type->getNumElements();
  if (idx >= size) {
    std::string errmsg = "index was " + std::to_string(idx) +
                         " array only has " + std::to_string(size) +
                         " elements.";
    return {Error(Error::Code::OutOfBounds, GetLoc(), errmsg)};
  }

  auto *array_ptr = env.instruction_builder->CreateConstInBoundsGEP1_32(
      array_slice_type, array_slice_ptr, 1);

  auto *element_ptr = env.instruction_builder->CreateConstInBoundsGEP1_32(
      array_type, array_ptr, idx);

  size_t slice_size = size - idx;
  auto *llvm_slice_size = env.instruction_builder->getInt64(slice_size);

  auto *slice_ptr = AllocateGlobal(Gensym(), slice_type, env);

  auto *slice_size_ptr = env.instruction_builder->CreateConstInBoundsGEP1_32(
      slice_type, slice_ptr, 0);
  auto *slice_ptr_ptr = env.instruction_builder->CreateConstInBoundsGEP1_32(
      slice_type, slice_ptr, 1);

  env.instruction_builder->CreateStore(llvm_slice_size, slice_size_ptr);
  env.instruction_builder->CreateStore(element_ptr, slice_ptr_ptr);
  return {slice_ptr};
}

auto Binop::CodegenLocalArrayAdd(llvm::StructType *array_slice_type,
                                 llvm::Value *array_slice_ptr,
                                 llvm::ConstantInt *index_ptr,
                                 const Environment &env) const
    -> Outcome<llvm::Value *, Error> {
  assert(array_slice_ptr != nullptr);
  assert(array_slice_type != nullptr);
  assert(index_ptr != nullptr);
  assert(env.current_function != nullptr);
  assert(llvm::cast<llvm::AllocaInst>(array_slice_ptr) != nullptr);

  auto *slice_size_type = env.instruction_builder->getInt64Ty();
  auto *slice_ptr_type = llvm::PointerType::get(
      *env.context, env.data_layout.getAllocaAddrSpace());
  auto *slice_type =
      llvm::StructType::get(*env.context, {slice_size_type, slice_ptr_type});

  auto *slice_index_one_type = array_slice_type->getTypeAtIndex(1);
  auto *array_type = llvm::cast<llvm::ArrayType>(slice_index_one_type);

  // #RULE arrays have a size property
  // #RULE all array access must be bounds checked
  size_t idx = index_ptr->getLimitedValue();
  size_t size = array_type->getNumElements();
  if (idx >= size) {
    std::string errmsg = "index was " + std::to_string(idx) +
                         " array only has " + std::to_string(size) +
                         " elements.";
    return {Error(Error::Code::OutOfBounds, GetLoc(), errmsg)};
  }

  auto *array_ptr = env.instruction_builder->CreateConstInBoundsGEP1_32(
      array_slice_type, array_slice_ptr, 1);

  auto *element_ptr = env.instruction_builder->CreateConstInBoundsGEP1_32(
      array_type, array_ptr, idx);

  size_t slice_size = size - idx;
  auto *llvm_slice_size = env.instruction_builder->getInt64(slice_size);

  auto *slice_ptr = AllocateLocal(Gensym(), slice_type, env);

  auto *slice_size_ptr = env.instruction_builder->CreateConstInBoundsGEP1_32(
      slice_type, slice_ptr, 0);
  auto *slice_ptr_ptr = env.instruction_builder->CreateConstInBoundsGEP1_32(
      slice_type, slice_ptr, 1);

  env.instruction_builder->CreateStore(llvm_slice_size, slice_size_ptr);
  env.instruction_builder->CreateStore(element_ptr, slice_ptr_ptr);

  return {slice_ptr};
}

auto Binop::CodegenLocalSliceAdd(SliceType *slice_type, llvm::Value *slice_ptr,
                                 llvm::ConstantInt *index_ptr,
                                 const Environment &env) const
    -> Outcome<llvm::Value *, Error> {
  assert(slice_type != nullptr);
  assert(slice_ptr != nullptr);
  assert(index_ptr != nullptr);

  auto llvm_slice_type_result = slice_type->Codegen(env);
  if (!llvm_slice_type_result) {
    return {llvm_slice_type_result.GetSecond()};
  }
  auto *llvm_slice_type =
      llvm::cast<llvm::StructType>(llvm_slice_type_result.GetFirst());

  auto llvm_slice_element_type_result = slice_type->pointee_type->Codegen(env);
  if (!llvm_slice_element_type_result) {
    return {llvm_slice_element_type_result.GetSecond()};
  }
  auto *llvm_slice_element_type = llvm_slice_element_type_result.GetFirst();

  auto *slice_size_type = llvm_slice_type->getElementType(0);
  auto *slice_size_ptr = env.instruction_builder->CreateConstInBoundsGEP1_32(
      llvm_slice_type, slice_ptr, 0);
  auto *slice_ptr_ptr = env.instruction_builder->CreateConstInBoundsGEP1_32(
      llvm_slice_type, slice_ptr, 1);

  auto *result_slice_ptr = AllocateLocal(Gensym(), llvm_slice_type, env);

  auto *slice_size =
      env.instruction_builder->CreateLoad(slice_size_type, slice_size_ptr);
}

/*
    env |- lhs : Tl, rhs : Tr, op : Tl -> Tr -> T
  --------------------------------------------------
                      env |- lhs op rhs : T
*/
auto Binop::TypecheckV(const Environment &env) const -> Outcome<Type *, Error> {
  auto lhs_result = left->Typecheck(env);
  if (!lhs_result) {
    return lhs_result;
  }

  auto rhs_result = right->Typecheck(env);
  if (!rhs_result) {
    return rhs_result;
  }

  auto binop = env.binops->Lookup(op);

  // #RULE: any operator with zero overloads cannot be used in
  // an infix expression.
  if (!binop || binop->second->NumOverloads() == 0) {
    std::string errmsg = std::string("unknown op: ") + op;
    return {Error(Error::Code::UnknownBinop, GetLoc(), errmsg)};
  }

  // #RULE Array types support pointer arithmetic by being 'cast'
  // as slices.
  auto *array_type = llvm::dyn_cast<ArrayType>(lhs_result.GetFirst());
  if ((array_type != nullptr) && (strcmp(op, "+") == 0)) {
    return TypecheckLHSArrayAdd(array_type, rhs_result.GetFirst(), env);
  }

  array_type = llvm::dyn_cast<ArrayType>(rhs_result.GetFirst());
  if ((array_type != nullptr) && (strcmp(op, "+") == 0)) {
    return TypecheckRHSArrayAdd(array_type, lhs_result.GetFirst(), env);
  }

  // #RULE Binops are overloaded on their argument types
  auto literal =
      binop->second->Lookup(lhs_result.GetFirst(), rhs_result.GetFirst());

  if (!literal) {
    std::string errmsg =
        std::string("could not find an implementation of ") + std::string(op) +
        " for given types, left: " + lhs_result.GetFirst()->ToString() +
        ", right: " + rhs_result.GetFirst()->ToString();
    return {Error(Error::Code::ArgTypeMismatch, GetLoc(), errmsg)};
  }

  return {literal->second->result_type};
}

auto Binop::Codegen(const Environment &env) const
    -> Outcome<llvm::Value *, Error> {
  // Get the type and value of both sides
  Outcome<Type *, Error> lhs_type_result(left->Typecheck(env));

  if (!lhs_type_result) {
    return {lhs_type_result.GetSecond()};
  }

  Outcome<llvm::Type *, Error> lhs_type_codegen =
      lhs_type_result.GetFirst()->Codegen(env);

  if (!lhs_type_codegen) {
    return {lhs_type_codegen.GetSecond()};
  }

  llvm::Type *lhs_type = lhs_type_codegen.GetFirst();

  Outcome<llvm::Value *, Error> lhs_value(left->Codegen(env));

  if (!lhs_value) {
    return lhs_value;
  }

  Outcome<Type *, Error> rhs_type_result(right->Typecheck(env));

  if (!rhs_type_result) {
    return {rhs_type_result.GetSecond()};
  }

  Outcome<llvm::Type *, Error> rhs_type_codegen =
      rhs_type_result.GetFirst()->Codegen(env);

  if (!rhs_type_codegen) {
    return {rhs_type_codegen.GetSecond()};
  }

  llvm::Type *rhs_type = rhs_type_codegen.GetFirst();

  Outcome<llvm::Value *, Error> rhs_value(right->Codegen(env));

  if (!rhs_value) {
    return rhs_value;
  }

  // find the operator present between both sides in the env
  llvm::Optional<std::pair<InternedString, BinopLiteral *>> binop =
      env.binops->Lookup(op);

  if (!binop) {
    std::string errmsg = std::string("unknown op: ") + op;
    return {Error(Error::Code::UnknownBinop, GetLoc(), errmsg)};
  }

  llvm::Optional<std::pair<std::pair<Type *, Type *>, BinopCodegen *>> literal;
  auto *pointer_type = llvm::dyn_cast<PointerType>(lhs_type_result.GetFirst());
  // if this is a pointer addition operation, we must pass the pointee_type
  // into the binop code generation function for the GEP instruction within.
  // we can also safely add a new implementation of pointer addition no
  // matter what the underlying type is, because the GEP instruction
  // handles the offset computation based upon the pointee_type itself.
  // it is only the structure of the BinopLiteral itself that is going to
  // fail to find an implementation for any arbitrary array type we can
  // define.
  if ((pointer_type != nullptr) && (strcmp(op, "+") == 0)) {
    Outcome<llvm::Type *, Error> pointee_type_result =
        pointer_type->pointee_type->Codegen(env);

    if (!pointee_type_result) {
      return {pointee_type_result.GetSecond()};
    }

    lhs_type = pointee_type_result.GetFirst();

    auto *int_type = llvm::dyn_cast<IntType>(rhs_type_result.GetFirst());
    if (int_type == nullptr) {
      std::string errmsg = std::string("lhs has pointer type: ") +
                           pointer_type->ToString() +
                           " however rhs is not an Int, rhs has type: " +
                           rhs_type_result.GetFirst()->ToString();
      return {Error(Error::Code::ArgTypeMismatch, GetLoc(), errmsg)};
    }

    literal = binop->second->Lookup(lhs_type_result.GetFirst(), int_type);

    if (!literal) {
      Type *int_ptr_type = env.types->GetPointerType(env.types->GetIntType());
      llvm::Optional<std::pair<std::pair<Type *, Type *>, BinopCodegen *>>
          ptr_add_binop = binop->second->Lookup(int_ptr_type, int_type);

      if (!ptr_add_binop) {
        FatalError("Couldn't find int ptr arithmetic binop!", __FILE__,
                   __LINE__);
      }

      BinopCodegenFn ptr_arith_fn = ptr_add_binop->second->generate;
      literal =
          binop->second->Register(lhs_type_result.GetFirst(), int_type,
                                  lhs_type_result.GetFirst(), ptr_arith_fn);
    }
  } else {
    // find the instance of the operator given the type of both sides
    literal = binop->second->Lookup(lhs_type_result.GetFirst(),
                                    rhs_type_result.GetFirst());
  }

  if (!literal) {
    std::string errmsg = std::string("could not find an implementation of ") +
                         std::string(op) + " for the given types, left: " +
                         lhs_type_result.GetFirst()->ToString() +
                         ", right: " + rhs_type_result.GetFirst()->ToString();
    return {Error(Error::Code::ArgTypeMismatch, GetLoc(), errmsg)};
  }

  // use the lhs and rhs values to generate the binop expression.
  return {literal->second->generate(lhs_type, lhs_value.GetFirst(), rhs_type,
                                    rhs_value.GetFirst(), env)};
}
} // namespace pink
