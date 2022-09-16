#include "type/PointerType.h"
#include "aux/Environment.h"

#include "llvm/IR/DerivedTypes.h"


namespace pink {
  PointerType::PointerType(Type* pointee_type)
    : Type(Type::Kind::Pointer), pointee_type(pointee_type)
  {

  }

  PointerType::~PointerType()
  {

  }

  bool PointerType::classof(const Type* t)
  {
    return t->getKind() == Type::Kind::Pointer;
  }

  bool PointerType::EqualTo(Type* other)
  {
    bool equal = true;
    
    if (PointerType* pt = llvm::dyn_cast<PointerType>(other))
    {
      if (pt->pointee_type != this->pointee_type)
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

  std::string PointerType::ToString()
  {
    std::string result;

    result += pointee_type->ToString();
    result += "*";

    return result;
  }

  Outcome<llvm::Type*, Error> PointerType::Codegen(const Environment& env)
  {
    return Outcome<llvm::Type*, Error>(llvm::PointerType::getUnqual(*env.context));
  }

}
