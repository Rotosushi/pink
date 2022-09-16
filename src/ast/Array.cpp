
#include "ast/Array.h"

#include "aux/Environment.h"

#include "support/LLVMValueToString.h"

#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Constants.h"

namespace pink {
  Array::Array(Location location, std::vector<std::unique_ptr<Ast>> members)
    : Ast(Ast::Kind::Array, location), members(std::move(members))
  {

  }

  Array::~Array()
  {

  }

  bool Array::classof(const Ast* ast)
  {
    return ast->getKind() == Ast::Kind::Array;
  }

  std::unique_ptr<Ast> Array::Clone()
  {
    std::vector<std::unique_ptr<Ast>> membs;

    for (std::unique_ptr<Ast>& memb : members)
    {
      membs.emplace_back(memb->Clone());
    }

    return std::unique_ptr<Array>(new Array(loc, std::move(membs)));
  }

  std::string Array::ToString()
  {
    std::string result;
    result += "[";
    
    size_t i = 0;
    for (std::unique_ptr<Ast>& memb : members)
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
  Outcome<Type*, Error> Array::GetypeV(const Environment& env)
  {
    size_t i = 0;
    std::vector<Type*> membTys;
    for (std::unique_ptr<Ast>& memb : members)
    {
      Outcome<Type*, Error> memb_result = memb->Getype(env);

      if (!memb_result)
        return memb_result;

      membTys.push_back(memb_result.GetOne());

      if (memb_result.GetOne() != membTys[0])
      { 
        std::string errmsg = std::string("at position: ")
                           + std::to_string(i)
                           + ", expected type: "
                           + membTys[0]->ToString()
                           + ", actual type: "
                           + memb_result.GetOne()->ToString();
        return Outcome<Type*, Error>(Error(Error::Code::ArrayMemberTypeMismatch, loc));
      }
      i += 1;
    }    
    
    return Outcome<Type*, Error>(env.types->GetArrayType(members.size(), membTys[0]));
  }

  /*  The value of an array is a llvm::ConstantArray of the same length 
   *  and type. As long as the array can be typed.
   *
   *
   *
   */
  Outcome<llvm::Value*, Error> Array::Codegen(const Environment& env)
  {
    Outcome<Type*, Error> type_result = this->Getype(env);
    
    if (!type_result)
    {
      return Outcome<llvm::Value*, Error>(type_result.GetTwo());
    } 
    
    Outcome<llvm::Type*, Error> llvm_type = type_result.GetOne()->Codegen(env);

    if (!llvm_type)
    {
      return Outcome<llvm::Value*, Error>(llvm_type.GetTwo());
    }

    // if should be safe to cast the type of this term to an ArrayType,
    // given that this term represents arrays. we should be able to expect 
    // that the type of this is an ArrayType.
    llvm::ArrayType* array_type = llvm::cast<llvm::ArrayType>(llvm_type.GetOne());

    std::vector<llvm::Constant*> cmembers;

    for (std::unique_ptr<Ast>& member : members)
    {
      Outcome<llvm::Value*, Error> member_result = member->Codegen(env);

      if (!member_result)
      {
        return Outcome<llvm::Value*, Error>(member_result.GetTwo());
      }

      llvm::Constant* cmember = llvm::dyn_cast<llvm::Constant>(member_result.GetOne());

      if (cmember == nullptr)
      {
        std::string errmsg = std::string("llvm::Value* is: ")
                           + LLVMValueToString(member_result.GetOne());
        return Outcome<llvm::Value*, Error>(Error(Error::Code::NonConstArrayInit, loc, errmsg));
      }

      cmembers.push_back(cmember);
    }

    return Outcome<llvm::Value*, Error>(llvm::ConstantArray::get(array_type, cmembers));
  }
}


















/**/
