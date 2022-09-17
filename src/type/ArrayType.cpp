#include "type/ArrayType.h"

#include "llvm/IR/DerivedTypes.h"

namespace pink {
  ArrayType::ArrayType(size_t size, Type* member_type)
    : Type(Type::Kind::Array), size(size), member_type(member_type)
  {

  }

  ArrayType::~ArrayType()
  {

  }

  bool ArrayType::classof(const Type* t)
  {
    return t->getKind() == Type::Kind::Array;
  }

  bool ArrayType::EqualTo(Type* other)
  {
    bool equal = true;

    if (ArrayType* at = llvm::dyn_cast<ArrayType>(other))
    {
      if (at->size != size || at->member_type != member_type)
      {
        equal = false;
      }
    }
    else
    {
      equal = false;
    }

    return equal;
  }

  std::string ArrayType::ToString()
  {
    std::string result;

    result += "[";
  
    result += member_type->ToString();

    result += " x ";

    result += std::to_string(size);
    
    result += "]";

    return result;
  }

  Outcome<llvm::Type*, Error> ArrayType::Codegen(const Environment& env)
  {
    Outcome<llvm::Type*, Error> member_result = member_type->Codegen(env);

    if (!member_result)
      return member_result;

    return Outcome<llvm::Type*, Error>(llvm::ArrayType::get(member_result.GetFirst(), size));
  }
}
