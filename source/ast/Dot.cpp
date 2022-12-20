#include "ast/Dot.h"
#include "ast/Int.h"
#include "type/TupleType.h"

#include "aux/Environment.h"

#include "kernel/LoadValue.h"

#include "visitor/AstVisitor.h"

namespace pink {
Dot::Dot(const Location &location, std::unique_ptr<Ast> left,
         std::unique_ptr<Ast> right)
    : Ast(Ast::Kind::Dot, location), left(std::move(left)),
      right(std::move(right)) {}

void Dot::Accept(const ConstAstVisitor *visitor) const { visitor->Visit(this); }

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
  auto *left_type = left_typecheck_result.GetFirst();

  Outcome<Type *, Error> right_typecheck_result = right->Typecheck(env);
  if (!right_typecheck_result) {
    return right_typecheck_result;
  }
  auto *right_type = right_typecheck_result.GetFirst();

  auto *tuple_type = llvm::dyn_cast<TupleType>(left_type);
  if (left_type == nullptr) {
    std::string errmsg = std::string("left has type: ") + left_type->ToString();
    return {Error(Error::Code::DotLeftIsNotATuple, GetLoc(), errmsg)};
  }
  assert(tuple_type != nullptr);

  // #RULE We cannot typecheck a tuple index against a runtime value.
  // As it violates static (compile time) typing. So we require the rhs of dot
  // member access to be an integer literal. (integral literal eventually)
  auto *index = llvm::dyn_cast<Int>(right.get());
  if (index == nullptr) {
    std::string errmsg =
        std::string("right has type: ") + right_type->ToString();

    return {Error(Error::Code::DotRightIsNotAnInt, GetLoc(), errmsg)};
  }

  auto index_value = static_cast<size_t>(index->GetValue());
  if (index_value > tuple_type->member_types.size()) {
    std::string errmsg = std::string("tuple has ") +
                         std::to_string(tuple_type->member_types.size()) +
                         " elements, index is: " + std::to_string(index_value);
    return {Error(Error::Code::DotIndexOutOfRange, GetLoc(), errmsg)};
  }

  return {tuple_type->member_types[index_value]};
}

auto Dot::Codegen(const Environment &env) const
    -> Outcome<llvm::Value *, Error> {
  assert(GetType() != nullptr);

  Outcome<llvm::Value *, Error> left_codegen_result = left->Codegen(env);
  if (!left_codegen_result) {
    return left_codegen_result;
  }
  llvm::Value *left_value = left_codegen_result.GetFirst();

  auto *index = llvm::dyn_cast<Int>(right.get());
  if (index == nullptr) {
    std::string errmsg =
        std::string("right has type: ") + right->GetType()->ToString();
    FatalError(errmsg, __FILE__, __LINE__);
  }

  auto *left_type = llvm::dyn_cast<TupleType>(left->GetType());
  assert(left_type != nullptr);

  auto index_value = static_cast<size_t>(index->GetValue());
  if (index_value > left_type->member_types.size()) {
    std::string errmsg = std::string("tuple has ") +
                         std::to_string(left_type->member_types.size()) +
                         " elements, index is: " + std::to_string(index_value);
    FatalError(errmsg, __FILE__, __LINE__);
  }

  auto *struct_type = llvm::cast<llvm::StructType>(left_type->ToLLVM(env));
  assert(struct_type != nullptr);

  llvm::Value *gep = env.instruction_builder->CreateConstGEP2_32(
      struct_type, left_value, 0, index_value);

  llvm::Type *member_type = struct_type->getTypeAtIndex(index_value);
  return LoadValue(member_type, gep, env);
}
} // namespace pink
