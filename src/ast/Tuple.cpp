#include "ast/Tuple.h"
#include "aux/Environment.h"

#include "support/LLVMValueToString.h"

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

  Outcome<Type*, Error> Tuple::GetypeV(const Environment& env)
  {
    std::vector<Type*> member_types;

    for (auto& member : members)
    {
      Outcome<Type*, Error> member_getype_result = member->Getype(env);
    
      if (!member_getype_result)
        return member_getype_result;

      member_types.push_back(member_getype_result.GetFirst());
    }

    return Outcome<Type*, Error>(env.types->GetTupleType(member_types));
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
    *  This works great, now we just need to test this against a few more things: 
    *  1) tuple with array members
    *  2) array with tuple members
    *  3) passing tuples into functions
    *  4) returning tuples from functions
    *  5) arrays with non-const member initializers
    *  6) tuples with non-const member initializers
    *
    *  1) I think as long as we ask llvm to construct a ConstantStruct 
    *     with a ConstantArray initializer we can declare such a struct 
    *     in compile space as we already do. The tricky parts of this are 
    *     A) initializing the local variable with the struct contents,
    *        when the members of the struct are not all single value sized.
    *     B) initializing the member of the local variable if the member 
    *        is itself an aggregate of non single value sized types.
    *
    *     the good news is that B looks a lot like A, so maybe we can use 
    *     the recursive structure of aggregate membership to write a 
    *     recursive solution.
    *     more good news is that such a recursive solution may also be 
    *     able to handle case 2, and the subcases of 2 as well. (noticing that
    *     2A is equivalent to 1A with 'tuple' replaced by 'array')
    *  
    *  2) see 1
    *
    *  3) a structures into functions as arguments by value can be accomplished 
    *     by adding the parameter attribute "byval(<ty>)" on the argument which 
    *     is meant to pass the structure object. then, when we define
    *     a function type with an argument of structure type we modify that
    *     argument to a pointer to the same structure type for llvm.
    *     A) if the function is meant to pass the address of a structure
    *        already, we do not want to add the byval attribute, as pointers 
    *        are single value sized.
    *
    *  4) if a function is meant to return a structure as it's return value, 
  *       we can support that in llvm by modifying the return type of the 
    *     llvm function type to return void, and adding an extra argument 
    *     whose type is a structure pointer with a parameter attribute of 
    *     sret(<ty>), where ty is the structure type itself. then when 
    *     we attempt to make a function call of such a function we must 
    *     allocate memory in the caller for the structure, before passing  
    *     a pointer to that memory and all the other arguments as usual to 
    *     a function call instruction.
    *
    *  5) unfortunately for both 5 and 6 we have to modify the return type of 
    *     Codegen itself. For if we want to support aggregate initialization we 
    *     must return an aggregate of llvm::Value*'s via the result of
    *     a Codegen call. we have somewhat of a choice however, as we could 
    *     either modify the llvm::Value* to instead be an aggregate all the
    *     time, and when size() == 1 we know it's "not" an aggregate value.
    *     or we could define a new class which wraps this behavior in a few 
    *     class methods. (which are in the end just accessors to an existing 
    *     aggregate, probably std::vector)
   */
  Outcome<llvm::Value*, Error> Tuple::Codegen(const Environment& env)
  {
    std::vector<llvm::Constant*> tuple_elements; 

    Outcome<Type*, Error> tuple_getype_result = this->Getype(env);

    if (!tuple_getype_result)
      return Outcome<llvm::Value*, Error>(tuple_getype_result.GetSecond());

    Outcome<llvm::Type*, Error> tuple_type_codegen_result = tuple_getype_result.GetFirst()->Codegen(env);

    if (!tuple_type_codegen_result)
      return Outcome<llvm::Value*, Error>(tuple_type_codegen_result.GetSecond());

    llvm::StructType* tuple_type = llvm::dyn_cast<llvm::StructType>(tuple_type_codegen_result.GetFirst());

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

      if (llvm::Constant* initializer = llvm::dyn_cast<llvm::Constant>(initializer_result.GetFirst()))
      {
        tuple_elements.push_back(initializer);
      }
      else
      {
        std::string errmsg = std::string("value is: ")
                           + LLVMValueToString(initializer);
        return Outcome<llvm::Value*, Error>(Error(Error::Code::NonConstTupleInit, loc, errmsg));
      }
    }

    return llvm::ConstantStruct::get(tuple_type, tuple_elements);
  }
}
