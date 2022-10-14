#include "ast/Dot.h"
#include "ast/Int.h"
#include "type/TupleType.h"

#include "aux/Environment.h"

#include "visitor/AstVisitor.h"

namespace pink {
Dot::Dot(const Location &location, std::unique_ptr<Ast> left,
         std::unique_ptr<Ast> right)
    : Ast(Ast::Kind::Dot, location), left(std::move(left)),
      right(std::move(right)) {}

void Dot::Accept(AstVisitor *visitor) { visitor->Visit(this); }

auto Dot::classof(const Ast *ast) -> bool {
  return ast->GetKind() == Ast::Kind::Dot;
}

auto Dot::ToString() const -> std::string {
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
auto Dot::TypecheckV(const Environment &env) const -> Outcome<Type *, Error> {
  Outcome<Type *, Error> left_typecheck_result = left->Typecheck(env);

  if (!left_typecheck_result) {
    return left_typecheck_result;
  }

  Outcome<Type *, Error> right_typecheck_result = right->Typecheck(env);

  if (!right_typecheck_result) {
    return right_typecheck_result;
  }

  auto *left_type = llvm::dyn_cast<TupleType>(left_typecheck_result.GetFirst());

  if (left_type == nullptr) {
    std::string errmsg = std::string("left has type: ") +
                         left_typecheck_result.GetFirst()->ToString();
    return {Error(Error::Code::DotLeftIsNotAStruct, GetLoc(), errmsg)};
  }

  auto *index = llvm::dyn_cast<Int>(right.get());

  if (index == nullptr) {
    std::string errmsg = std::string("right has type: ") +
                         right_typecheck_result.GetFirst()->ToString();

    return {Error(Error::Code::DotRightIsNotAnInt, GetLoc(), errmsg)};
  }

  auto index_value = static_cast<size_t>(index->GetValue());
  if (index_value > left_type->member_types.size()) {
    std::string errmsg = std::string("tuple has ") +
                         std::to_string(left_type->member_types.size()) +
                         " elements, index is: " + std::to_string(index_value);
    return {Error(Error::Code::DotIndexOutOfRange, GetLoc(), errmsg)};
  }

  return {left_type->member_types[index_value]};
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
auto Dot::Codegen(const Environment &env) const
    -> Outcome<llvm::Value *, Error> {
  Outcome<Type *, Error> left_typecheck_result = left->Typecheck(env);

  if (!left_typecheck_result) {
    return {left_typecheck_result.GetSecond()};
  }

  Outcome<Type *, Error> right_typecheck_result = right->Typecheck(env);

  if (!right_typecheck_result) {
    return {right_typecheck_result.GetSecond()};
  }

  auto *left_type = llvm::dyn_cast<TupleType>(left_typecheck_result.GetFirst());

  if (left_type == nullptr) {
    std::string errmsg = std::string("left has type: ") +
                         left_typecheck_result.GetFirst()->ToString();
    return {Error(Error::Code::DotLeftIsNotAStruct, GetLoc(), errmsg)};
  }

  Outcome<llvm::Type *, Error> left_type_codegen_result =
      left_type->Codegen(env);

  if (!left_type_codegen_result) {
    return {left_type_codegen_result.GetSecond()};
  }

  Outcome<llvm::Value *, Error> left_codegen_result = left->Codegen(env);

  if (!left_codegen_result) {
    return left_codegen_result;
  }

  llvm::Value *left_value = left_codegen_result.GetFirst();

  auto *index = llvm::dyn_cast<Int>(right.get());

  if (index == nullptr) {
    std::string errmsg = std::string("right has type: ") +
                         right_typecheck_result.GetFirst()->ToString();
    return {Error(Error::Code::DotRightIsNotAnInt, GetLoc(), errmsg)};
  }

  auto index_value = static_cast<size_t>(index->GetValue());
  if (index_value > left_type->member_types.size()) {
    std::string errmsg = std::string("tuple has ") +
                         std::to_string(left_type->member_types.size()) +
                         " elements, index is: " + std::to_string(index_value);
    return {Error(Error::Code::DotIndexOutOfRange, GetLoc(), errmsg)};
  }

  auto *struct_t =
      llvm::cast<llvm::StructType>(left_type_codegen_result.GetFirst());

  llvm::Value *gep = env.instruction_builder->CreateConstGEP2_32(
      struct_t, left_value, 0, index_value);

  llvm::Type *member_type = struct_t->getTypeAtIndex(index_value);
  llvm::Value *result = nullptr;

  // what I am realizing, is that this check right here is exactly why
  // c/cpp has value categories. that is glvalue, prvalue, xvalue, lvalue, and
  // rvalue. As in, this check could be replaced with value_type->is_lvalue
  // or something like that.
  if (!env.flags->WithinAddressOf() && !env.flags->OnTheLHSOfAssignment() &&
      member_type->isSingleValueType()) {
    result = env.instruction_builder->CreateLoad(member_type, gep);
  } else {
    result = gep;
  }

  return {result};
}
} // namespace pink
