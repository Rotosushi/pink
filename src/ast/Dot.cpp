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

  std::unique_ptr<Ast> Dot::Clone()
  {
    return std::make_unique<Dot>(loc, left->Clone(), right->Clone());
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
   *
   *  we only have anonymous structure types right now, that is Tuples.
   *  so we only need to support 1.
   *
   *  to support one we basically need to get the type of the left hand side,
   *  and attempt to cast it to a structure type, if we can't that's a type
   *  error. if we can, then we attempt to cast the right hand side to an
   *  integer literal, if we cant, then since we cannot know the offset at
   *  compile time we cannot compute the type at compile time, thus this is 
   *  also an error. however if right is a literal integer, then we can use 
   *  that integer to index the struct type of the left hand side to compute 
   *  the result type of the Dot operator.
   *
   */
  Outcome<Type*, Error> Dot::GetypeV(std::shared_ptr<Environment> env)
  {
    Outcome<Type*, Error> left_getype_result = left->Getype(env);

    if (!left_getype_result)
      return left_getype_result;

    TupleType* left_type = llvm::dyn_cast<TupleType>(left_getype_result.GetOne());

    if (left_type == nullptr)
    {
      Error error(Error::Code::DotLeftIsNotAStruct, loc);
      return Outcome<Type*, Error>(error);
    }

    Int* index = llvm::dyn_cast<Int>(right.get());

    if (index == nullptr)
    {
      Error error(Error::Code::DotRightIsNotAnInt, loc);
      return Outcome<Type*, Error>(error);
    }

    if (index->value > left_type->member_types.size())
    {
      Error error(Error::Code::DotIndexOutOfRange, loc);
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
  Outcome<llvm::Value*, Error> Dot::Codegen(std::shared_ptr<Environment> env)
  {
    Outcome<Type*, Error> left_getype_result = left->Getype(env);

    if (!left_getype_result)
      return Outcome<llvm::Value*, Error>(left_getype_result.GetTwo());

    TupleType* left_type = llvm::dyn_cast<TupleType>(left_getype_result.GetOne());

    if (left_type == nullptr)
    {
      Error error(Error::Code::DotLeftIsNotAStruct, loc);
      return Outcome<llvm::Value*, Error>(error);
    }

    Outcome<llvm::Type*, Error> left_type_codegen_result = left_type->Codegen(env);

    if (!left_type_codegen_result)
      return Outcome<llvm::Value*, Error>(left_type_codegen_result.GetTwo());


    llvm::StructType* struct_t = llvm::cast<llvm::StructType>(left_type_codegen_result.GetOne());

    Outcome<llvm::Value*, Error> left_codegen_result = left->Codegen(env);

    if (!left_codegen_result)
      return left_codegen_result;
    
    llvm::Value* left_value = left_codegen_result.GetOne();

    Int* index = llvm::dyn_cast<Int>(right.get());

    if (index == nullptr)
    {
      Error error(Error::Code::DotRightIsNotAnInt, loc);
      return Outcome<llvm::Value*, Error>(error);
    }

    if (index->value > left_type->member_types.size())
    {
      Error error(Error::Code::DotIndexOutOfRange, loc);
      return Outcome<llvm::Value*, Error> (error);
    }

    llvm::Value* result = nullptr;
    llvm::Value* gep = env->builder->CreateConstGEP2_32(struct_t, left_value, 0, index->value);
    
    if (!env->flags->WithinAddressOf() 
     && !env->flags->OnTheLHSOfAssignment()
     && struct_t->getTypeAtIndex(index->value)->isSingleValueType())
      result = env->builder->CreateLoad(struct_t->getTypeAtIndex(index->value), gep);
    else 
      result = gep;

    return Outcome<llvm::Value*, Error>(result);
  }
}
