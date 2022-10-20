
#include "ast/Array.h"

#include "aux/Environment.h"

#include "support/LLVMValueToString.h"

#include "visitor/AstVisitor.h"

#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"

namespace pink {
Array::Array(const Location &location,
             std::vector<std::unique_ptr<Ast>> members)
    : Ast(Ast::Kind::Array, location), members(std::move(members)) {}

void Array::Accept(AstVisitor *visitor) const { visitor->Visit(this); }

auto Array::classof(const Ast *ast) -> bool {
  return ast->GetKind() == Ast::Kind::Array;
}

auto Array::ToString() const -> std::string {
  std::string result;
  result += "[";

  size_t idx = 0;
  for (const auto &memb : members) {
    result += memb->ToString();

    if (idx < (members.size() - 1)) {
      result += ", ";
    }

    idx++;
  }

  result += "]";
  return result;
}

/*  The type of an array is the type of one of it's members
 *  and the length of the array, as long as every one of
 *  it's members are the same type.
 *
 *
 *
 *
 */
auto Array::TypecheckV(const Environment &env) const -> Outcome<Type *, Error> {
  size_t idx = 0;
  std::vector<Type *> membTys;
  for (const auto &memb : members) {
    Outcome<Type *, Error> memb_result = memb->Typecheck(env);

    if (!memb_result) {
      return memb_result;
    }

    membTys.push_back(memb_result.GetFirst());

    if (memb_result.GetFirst() != membTys[0]) {
      std::string errmsg =
          std::string("at position: ") + std::to_string(idx) +
          ", expected type: " + membTys[0]->ToString() +
          ", actual type: " + memb_result.GetFirst()->ToString();
      return {Error(Error::Code::ArrayMemberTypeMismatch, GetLoc(), errmsg)};
    }
    idx += 1;
  }

  return {env.types->GetArrayType(members.size(), membTys[0])};
}

/** the value of an array is a llvm::ConstantArray of the same length
 *  and type. As long as the array can be typed.
 *
 * \todo okay, the plan for arrays is to store them as a tuple of
 * the length and the array itself. Then what we return will be a
 * slice into the array.
 * that is we are going to construct
 * a new kind of pointer to an array, which is a 'slice'.
 * then any time we perform pointer arithmetic we can bounds
 * check the arithmetic. We will also add syntax to access the length
 * in user code, and this syntax will work on all slices as well.
 * we are also going to add a array indirection operator ([]) which will
 * perform the pointer arithmetic (now bounds checked) and the member
 * indirection operation as well. The bracket operator will only be available
 * to slices, where we can check the bounds. Then regular pointers
 * can still exist, they simply will not support
 * pointer arithmetic. thus regular pointers only support indirection,
 * and address of.
 * additionally, address of will always return a regular pointer.
 * the only way to construct a slice is by constructing an array.
 * the array name will be a slice. Passing an array name then
 * will result in passing a slice.
 * 1) add a slice type (or modify ArrayType to SliceType)
 * 2) store arrays as tuples of their length and the array itself
 * 3) add a bounds check to pointer arithmetic
 * 4) add the [] operator (probably in the same place as the . operator)
 *
 *
 *
 */
auto Array::Codegen(const Environment &env) const
    -> Outcome<llvm::Value *, Error> {
  // this can only happen if we don't call Typecheck
  // on an Ast before calling Codegen on it. So it's
  // not impossible to have happen.
  assert(GetType() != nullptr);
  // We are generating code for an Array Ast, thus we can
  // observe Array::Typecheck an see that it returns an
  // ArrayType. if this ever causes UB I am never using a
  // static cast again, as this is the most amount of
  // information I can even imagine having about what the
  // Derived Type would be when given a Base Type Pointer.
  auto *array_type = llvm::cast<ArrayType>(GetType());

  std::vector<llvm::Constant *> cmembers;

  for (const auto &member : members) {
    Outcome<llvm::Value *, Error> member_result = member->Codegen(env);
    if (!member_result) {
      return {member_result.GetSecond()};
    }

    auto *cmember = llvm::dyn_cast<llvm::Constant>(member_result.GetFirst());
    if (cmember == nullptr) {
      std::string errmsg =
          std::string(
              "llvm::Value* is not an llvm::Constant*; The Value* is: ") +
          LLVMValueToString(member_result.GetFirst());
      return {Error(Error::Code::NonConstArrayInit, GetLoc(), errmsg)};
    }

    cmembers.push_back(cmember);
  }

  auto array_type_result = array_type->Codegen(env);
  assert(array_type_result);

  auto *splice_type =
      llvm::cast<llvm::StructType>(array_type_result.GetFirst());

  auto *first_type = splice_type->getTypeAtIndex((unsigned)0);
  auto *second_type = splice_type->getTypeAtIndex((unsigned)1);

  auto *llvm_integer_type = llvm::cast<llvm::IntegerType>(first_type);
  auto *llvm_array_type = llvm::cast<llvm::ArrayType>(second_type);

  auto *constant_array = llvm::ConstantArray::get(llvm_array_type, cmembers);

  auto *size = llvm::ConstantInt::get(llvm_integer_type, array_type->size);

  auto *struct_type = llvm::StructType::get(
      *env.context, {llvm_integer_type, llvm_array_type}, false);

  return {llvm::ConstantStruct::get(struct_type, {size, constant_array})};
}
} // namespace pink

/**/
