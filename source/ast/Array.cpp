
#include "ast/Array.h"

#include "aux/Environment.h"

#include "support/LLVMValueToString.h"

#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"

namespace pink {

/*
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
*/

/*  The type of an array is the type of it's first member
 *  and the length of the array, as long as every one of
 *  it's members are the same type.
 
auto Array::TypecheckV(const Environment &env) const -> Outcome<Type *, Error> {
  size_t idx = 0;
  std::vector<Type *> member_types;

  for (const auto &memb : members) {
    Outcome<Type *, Error> member_type_result = memb->Typecheck(env);
    if (!member_type_result) {
      return member_type_result;
    }
    auto *member_type = member_type_result.GetFirst();

    member_types.push_back(member_type);

    if (member_type != member_types[0]) {
      std::string errmsg = std::string("at position: ") + std::to_string(idx) +
                           ", expected type: " + member_types[0]->ToString() +
                           ", actual type: " + member_type->ToString();
      return {Error(Error::Code::ArrayMemberTypeMismatch, GetLoc(), errmsg)};
    }
    idx += 1;
  }

  return {env.types->GetArrayType(members.size(), member_types[0])};
}
*/

/** the value of an array is a llvm::ConstantArray of the same length
 *  and type. As long as the array can be typed.
 *
 * \todo refactor array to return a reference to a new allocation of an
 * array. this allows us to support constructing a local array out of
 * local variables.
 *
 * \todo finish adding the construction of pointers into an array with
 * slices.
 *
 *
 *
 
auto Array::Codegen(const Environment &env) const
    -> Outcome<llvm::Value *, Error> {
  assert(GetType() != nullptr);
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

  auto *struct_type = llvm::cast<llvm::StructType>(array_type->ToLLVM(env));

  auto *first_type = struct_type->getTypeAtIndex((unsigned)0);
  auto *second_type = struct_type->getTypeAtIndex((unsigned)1);

  auto *llvm_integer_type = llvm::cast<llvm::IntegerType>(first_type);
  auto *llvm_array_type = llvm::cast<llvm::ArrayType>(second_type);

  auto *constant_array = llvm::ConstantArray::get(llvm_array_type, cmembers);
  auto *size = llvm::ConstantInt::get(llvm_integer_type, array_type->size);

  auto *array_layout = llvm::StructType::get(
      *env.context, {llvm_integer_type, llvm_array_type}, false);

  return {llvm::ConstantStruct::get(array_layout, {size, constant_array})};
}
*/
} // namespace pink