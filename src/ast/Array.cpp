
#include "ast/Array.h"

#include "aux/Environment.h"

#include "support/LLVMValueToString.h"

#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Constants.h"

namespace pink {
  Array::Array(const Location& location, std::vector<std::unique_ptr<Ast>> members)
    : Ast(Ast::Kind::Array, location), members(std::move(members))
  {

  }

  auto Array::classof(const Ast* ast) -> bool
  {
    return ast->getKind() == Ast::Kind::Array;
  }

  auto Array::ToString() const -> std::string
  {
    std::string result;
    result += "[";
    
    size_t i = 0;
    for (const auto& memb : members)
    {
      result += memb->ToString();

      if (i < (members.size() - 1))
        result += ", ";

      i++;
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
  auto Array::GetypeV(const Environment& env) const -> Outcome<Type*, Error>
  {
    size_t idx = 0;
    std::vector<Type*> membTys;
    for (const auto& memb : members)
    {
      Outcome<Type*, Error> memb_result = memb->Getype(env);

      if (!memb_result)
      {
        return memb_result;
      }

      membTys.push_back(memb_result.GetFirst());

      if (memb_result.GetFirst() != membTys[0])
      { 
        std::string errmsg = std::string("at position: ")
                           + std::to_string(idx)
                           + ", expected type: "
                           + membTys[0]->ToString()
                           + ", actual type: "
                           + memb_result.GetFirst()->ToString();
        return {Error(Error::Code::ArrayMemberTypeMismatch, loc)};
      }
      idx += 1;
    }    
    
    return {env.types->GetArrayType(members.size(), membTys[0])};
  }

  /*  The value of an array is a llvm::ConstantArray of the same length 
   *  and type. As long as the array can be typed.
   *
   *
   *
   */
  auto Array::Codegen(const Environment& env) const -> Outcome<llvm::Value*, Error>
  {
    assert(type != nullptr);
    
    Outcome<llvm::Type*, Error> llvm_type = type->Codegen(env);

    if (!llvm_type)
    {
      return {llvm_type.GetSecond()};
    }

    // if should be safe to cast the type of this term to an ArrayType,
    // given that this term represents arrays. we should be able to expect 
    // that the type of this is an ArrayType.
    auto* array_type = llvm::cast<llvm::ArrayType>(llvm_type.GetFirst());

    std::vector<llvm::Constant*> cmembers;

    for (const auto& member : members)
    {
      Outcome<llvm::Value*, Error> member_result = member->Codegen(env);

      if (!member_result)
      {
        return {member_result.GetSecond()};
      }

      auto* cmember = llvm::dyn_cast<llvm::Constant>(member_result.GetFirst());

      if (cmember == nullptr)
      {
        std::string errmsg = std::string("llvm::Value* is not an llvm::Constant*; The Value* -> ")
                           + LLVMValueToString(member_result.GetFirst());
        return {Error(Error::Code::NonConstArrayInit, loc, errmsg)};
      }

      cmembers.push_back(cmember);
    }

    return {llvm::ConstantArray::get(array_type, cmembers)};
  }
}


















/**/
