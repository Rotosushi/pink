#include "ast/Tuple.h"
#include "aux/Environment.h"

namespace pink {
  Tuple::Tuple(Location loc, std::vector<std::unique_ptr<Ast>> members)
    : Ast(Ast::Kind::Tuple, loc), members(std::move(members))
  {

  }

  Tuple::~Tuple()
  {

  }

  bool Tuple::classof(const Ast* ast)
  {
    return ast->getKind() == Ast::Kind::Tuple;
  }

  std::unique_ptr<Ast> Tuple::Clone()
  {
    std::vector<std::unique_ptr<Ast>> m;

    for (auto& member : members)
      m.emplace_back(member->Clone());

    return std::make_unique<Tuple>(loc, std::move(m));
  }

  std::string Tuple::ToString()
  {
    std::string result("(");
    size_t i = 0, n = members.size();
    
    for (auto& member: members)
    {
      result += member->ToString();
      
      if (i < (n - 1))
        result += ", ";

      i++;
    } 
    result += ")";
    return result;
  }

  Outcome<Type*, Error> Tuple::GetypeV(std::shared_ptr<Environment> env)
  {
    std::vector<Type*> member_types;

    for (auto& member : members)
    {
      Outcome<Type*, Error> member_getype_result = member->Getype(env);
    
      if (!member_getype_result)
        return member_getype_result;

      member_types.push_back(member_getype_result.GetOne());
    }

    return Outcome<Type*, Error>(env->types->GetTupleType(member_types));
  }

  /*
   *  #TODO: the current method we have of constructing arrays and tuples 
   *  works only if all initializers are constants. This works to get our 
   *  some of the semantics working, however we are missing the ability to 
   *  declare an array or tuple and initializing it with the contents of a 
   *  local or global variable. to accomplish this we could not use either 
   *  llvm::ConstantArray or llvm::ConstantStruct, but we must use
   *  a std::vector of the llvm::Value*'s to construct the initialization 
   *  instructions of the variable being declared. (or argument being passed).
   *
    *  #TODO: we currently have no way of getting pointers to specific elements
    *  within a tuple. which is critical to using them. there are a few
    *  distinct situations where we need to retrieve a pointer to a given
    *  element.
    *  1) the dot operator: "tuple.2"
    *  2) the dot operator plus the address of operator "&tuple.2"
    *  3) if we have a pointer to a tuple, then we need to perform 
    *     a step of indirection before we can compute either 
    *     1 or 2.
    *
    *  in practice all of 1,2,3, are accomplished via the GEP instruction,
    *  the main difference between the GEP of a struct and a GEP of an 
    *  array is that the array has one datatype, so we can compute the 
    *  GEP for any given element just by knowing the type of the first 
    *  element. This is why we can allow arrays to decompose into pointers.
    *  however for a tuple's GEP instruction we must pass in the type of the 
    *  tuple itself as the first argument to the GEP instruction.
    *
    *  this means this is what is required of the dot operator. because of this
    *  requirement we cannot decompose tuple's from the perspective of
    *  codegeneration. and that means we cannot conveniently define a binary 
    *  operator to accomplish the tasks of computing pointers, like we could 
    *  with array types. This means the dot operator must be parsed
    *  specifically, just like the assignment operator, and it must be a node 
    *  within the AST just like the assignment operator.    
    *
   */
  Outcome<llvm::Value*, Error> Tuple::Codegen(std::shared_ptr<Environment> env)
  {
    std::vector<llvm::Constant*> tuple_elements; 

    Outcome<Type*, Error> tuple_getype_result = this->Getype(env);

    if (!tuple_getype_result)
      return Outcome<llvm::Value*, Error>(tuple_getype_result.GetTwo());

    Outcome<llvm::Type*, Error> tuple_type_codegen_result = tuple_getype_result.GetOne()->Codegen(env);

    if (!tuple_type_codegen_result)
      return Outcome<llvm::Value*, Error>(tuple_type_codegen_result.GetTwo());

    llvm::StructType* tuple_type = llvm::dyn_cast<llvm::StructType>(tuple_type_codegen_result.GetOne());

    if (tuple_type == nullptr)
    {
      FatalError("Tuple doesn't have a tuple type!", __FILE__, __LINE__);
      return Outcome<llvm::Value*, Error>(Error(Error::Code::None, loc));
    }

    for (auto& member : members)
    {
      Outcome<llvm::Value*, Error> initializer_result = member->Codegen(env);

      if (!initializer_result)
        return initializer_result;

      if (llvm::Constant* initializer = llvm::dyn_cast<llvm::Constant>(initializer_result.GetOne()))
      {
        tuple_elements.push_back(initializer);
      }
      else
      {
        return Outcome<llvm::Value*, Error>(Error(Error::Code::NonConstTupleInit, loc));
      }
    }

    return llvm::ConstantStruct::get(tuple_type, tuple_elements);
  }
}
