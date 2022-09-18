#include "ast/Dot.h"
#include "ast/Int.h"
#include "type/TupleType.h"

#include "aux/Environment.h"

namespace pink {
  Dot::Dot(Location location, std::unique_ptr<Ast> left, std::unique_ptr<Ast> right)
    : Ast(Ast::Kind::Dot, location), left(std::move(left)), right(std::move(right))
  {

  }

  Dot::~Dot()
  {

  }

  bool Dot::classof(const Ast* ast)
  {
    return ast->getKind() == Ast::Kind::Dot;
  }

  std::string Dot::ToString()
  {
    return left->ToString() + "." + right->ToString();
  }
  
  /*
   *  The dot operator computes pointers into structure types,
   *  this is done in a few ways, 
   *  1) if left is an anonymous struct type, then the right
   *      must be an Integer Type, the Result type is then the 
   *      type of the member at that particular offset.
   *      given that we must know the offset itself to compute 
   *      the type, we must restrict the right side to be an 
   *      integer literal directly, and no other node kinds are 
   *      allowed.
   *  2) if left is a named structure type, then we compute the 
   *     offset by way of a literal integer directly OR we can use 
   *     a variable directly, we can then lookup the offset of the 
   *     given name, by using a LUT held inside the named structure 
   *     type itself.
   *  3) we could extend the dot operator to work on arrays as well.
   *      rather simply, if we can cast the lhs to any pointer type,
   *      and the rhs to any integer type, then since we can compute
   *      the type, we can simply emit a pointer arithmetic instruction.
   *
   *  we only have anonymous structure types right now, that is Tuples.
   *  so we only need to support 1.
   *
   *  to support 1 we need to get the type of the left hand side,
   *  and attempt to cast it to a structure type, if we can't that's a type
   *  error. if we can, then we attempt to cast the right hand side to an
   *  integer literal, if we cant, then since we cannot know the offset at
   *  compile time we cannot compute the type at compile time, thus this is 
   *  also an error. however if right is a literal integer, then we can use 
   *  that integer to index the struct type of the left hand side to compute 
   *  the result type of the Dot operator.
   *
   */
  Outcome<Type*, Error> Dot::GetypeV(const Environment& env)
  {
    Outcome<Type*, Error> left_getype_result = left->Getype(env);

    if (!left_getype_result)
      return left_getype_result;

    Outcome<Type*, Error> right_getype_result = right->Getype(env);

    if (!right_getype_result)
      return right_getype_result;

    TupleType* left_type = llvm::dyn_cast<TupleType>(left_getype_result.GetFirst());

    if (left_type == nullptr)
    {
      std::string errmsg = std::string("left has type: ")
                         + left_getype_result.GetFirst()->ToString();
      Error error(Error::Code::DotLeftIsNotAStruct, loc, errmsg);
      return Outcome<Type*, Error>(error);
    }

    Int* index = llvm::dyn_cast<Int>(right.get());

    if (index == nullptr)
    {
      std::string errmsg = std::string("right has type: ")
                         + right_getype_result.GetFirst()->ToString();
      Error error(Error::Code::DotRightIsNotAnInt, loc, errmsg);
      return Outcome<Type*, Error>(error);
    }

    if (static_cast<size_t>(index->value) > left_type->member_types.size())
    {
      std::string errmsg = std::string("tuple has ")
                         + std::to_string(left_type->member_types.size())
                         + " elements, index is: "
                         + std::to_string(index->value);
      Error error(Error::Code::DotIndexOutOfRange, loc, errmsg);
      return Outcome<Type*, Error> (error);
    }

    Type* result_type = left_type->member_types[index->value];
    return Outcome<Type*, Error>(result_type);
  }

  /*
   *  The actual instructions associated with the dot operator are 
   *  a GEP, plus a Load, (though only if the type is loadable/storable)
   *
   *  in this case we need to only emit the load if we are NOT within an 
   *  address of expression OR if the element type is not loadable.
   *
   *  if we have a pointer to a structure, and not a struct itself then 
   *  it is convenient if we simply emit a load instruction to get to 
   *  the struct iself BEFORE we compute the regular dot operator semantics. 
   *
   *
   */
  Outcome<llvm::Value*, Error> Dot::Codegen(const Environment& env)
  {
    Outcome<Type*, Error> left_getype_result = left->Getype(env);

    if (!left_getype_result)
      return Outcome<llvm::Value*, Error>(left_getype_result.GetSecond());

    Outcome<Type*, Error> right_getype_result = right->Getype(env);
    
    if (!right_getype_result)
      return Outcome<llvm::Value*, Error>(right_getype_result.GetSecond());


    TupleType* left_type = llvm::dyn_cast<TupleType>(left_getype_result.GetFirst());

    if (left_type == nullptr)
    {
      std::string errmsg = std::string("left has type: ")
                         + left_getype_result.GetFirst()->ToString();
      Error error(Error::Code::DotLeftIsNotAStruct, loc, errmsg);
      return Outcome<llvm::Value*, Error>(error);
    }

    Outcome<llvm::Type*, Error> left_type_codegen_result = left_type->Codegen(env);

    if (!left_type_codegen_result)
      return Outcome<llvm::Value*, Error>(left_type_codegen_result.GetSecond());


    llvm::StructType* struct_t = llvm::cast<llvm::StructType>(left_type_codegen_result.GetFirst());

    Outcome<llvm::Value*, Error> left_codegen_result = left->Codegen(env);

    if (!left_codegen_result)
      return left_codegen_result;
    
    llvm::Value* left_value = left_codegen_result.GetFirst();

    Int* index = llvm::dyn_cast<Int>(right.get());

    if (index == nullptr)
    {
      std::string errmsg = std::string("right has type: ")
                         + right_getype_result.GetFirst()->ToString();
      Error error(Error::Code::DotRightIsNotAnInt, loc, errmsg);
      return Outcome<llvm::Value*, Error>(error);
    }

    if (static_cast<size_t>(index->value) > left_type->member_types.size())
    {
      std::string errmsg = std::string("tuple has ")
                         + std::to_string(left_type->member_types.size())
                         + " elements, index is: "
                         + std::to_string(index->value);
      Error error(Error::Code::DotIndexOutOfRange, loc, errmsg);
      return Outcome<llvm::Value*, Error> (error);
    }

    llvm::Value* result = nullptr;
    llvm::Value* gep = env.instruction_builder->CreateConstGEP2_32(struct_t, left_value, 0, index->value);
    llvm::Type* member_type = struct_t->getTypeAtIndex(index->value);

    if (!env.flags->WithinAddressOf() 
     && !env.flags->OnTheLHSOfAssignment()
     && member_type->isSingleValueType())
      result = env.instruction_builder->CreateLoad(member_type, gep);
    else 
      result = gep;

    return Outcome<llvm::Value*, Error>(result);
  }
}
