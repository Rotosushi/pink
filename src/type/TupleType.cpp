#include "type/TupleType.h"

#include "aux/Environment.h"

#include "llvm/IR/DerivedTypes.h"


namespace pink {
  TupleType::TupleType(std::vector<Type*> member_types)
    : Type(Type::Kind::Tuple), member_types(member_types)
  {

  }

  TupleType::~TupleType()
  {

  }

  bool TupleType::classof(const Type* type)
  {
    return type->getKind() == Type::Kind::Tuple;
  }

  bool TupleType::EqualTo(Type* other)
  {
    bool result = true;
    
    if (TupleType* tt = llvm::dyn_cast<TupleType>(other))
    {
      if (member_types.size() == tt->member_types.size())
      {
        for (size_t i = 0; i < member_types.size(); i++)
        {
          if (member_types[i] != tt->member_types[i])
          {
            result = false;
            break;
          }
        }  
      }
      else
      {
        result = false;
      }
    }
    else
    {
      result = false;
    }
    
    return result;
  }


  std::string TupleType::ToString()
  {
    std::string result = "(";
    for (size_t i = 0; i < member_types.size(); i++)
    {
      result += member_types[i]->ToString();
      if (i < (member_types.size() - 1))
      {
        result += ", ";
      }
    }
    result += ")";
    return result;
  }

  Outcome<llvm::Type*, Error> TupleType::Codegen(const Environment& env)
  {
    std::vector<llvm::Type*> member_llvm_types;
    
    for (Type* member_type : member_types)
    {
      Outcome<llvm::Type*, Error> member_type_codegen_result = member_type->Codegen(env);

      if (!member_type_codegen_result)
        return member_type_codegen_result;

      member_llvm_types.push_back(member_type_codegen_result.GetOne());
    }

    return llvm::StructType::get(*env.context, member_llvm_types);
  }
}



