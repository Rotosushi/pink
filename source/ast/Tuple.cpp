#include "ast/Tuple.h"
#include "aux/Environment.h"

#include "support/LLVMValueToString.h"

namespace pink {
Tuple::Tuple(const Location &location,
             std::vector<std::unique_ptr<Ast>> members)
    : Ast(Ast::Kind::Tuple, location), members(std::move(members)) {}

auto Tuple::classof(const Ast *ast) -> bool {
  return ast->GetKind() == Ast::Kind::Tuple;
}

auto Tuple::ToString() const -> std::string {
  std::string result("(");
  size_t idx = 0;
  size_t length = members.size();

  for (const auto &member : members) {
    result += member->ToString();

    if (idx < (length - 1)) {
      result += ", ";
    }

    idx++;
  }
  result += ")";
  return result;
}

auto Tuple::TypecheckV(const Environment &env) const -> Outcome<Type *, Error> {
  std::vector<Type *> member_types;

  // this is a perfect place for the std::transform algorithm,
  // except that we have the possiblity of failure on any given
  // call to Typecheck and std::tranform requires the unary
  // operation to have no side effects. thus, no possiblity of
  // handling the early return.
  for (const auto &member : members) {
    Outcome<Type *, Error> member_getype_result = member->Typecheck(env);

    if (!member_getype_result) {
      return member_getype_result;
    }

    member_types.push_back(member_getype_result.GetFirst());
  }

  return {env.types->GetTupleType(member_types)};
}

/**
 * \todo make tuple return an already alocated tuple.
 * this way we can handle constructing a tuple out
 * of constants and variables. returning the pointer
 * to the allocation as the result. However, this raises
 * a question. when we construct a global variable we
 * must have a name for the global. (it isn't necessary
 * for locals, but it makes the llvm ir a bit more readable.)
 * So how do we transmit that information to the tuple allocator
 * when it is only known to the bind term within the Ast?
 * the obvious solution is to just use a generated symbol.
 * but that reduces the legibility of the emitted llvm ir.
 * the way we have been transmitting information down the tree
 * is by way of the environment, so if bind stores the name, then
 * any term which needs to allocate can grab that name.
 * but then what about anonymous terms? (temporary values)
 */
auto Tuple::Codegen(const Environment &env) const
    -> Outcome<llvm::Value *, Error> {
  assert(GetType() != nullptr);

  std::vector<llvm::Constant *> tuple_elements;

  auto *tuple_type = llvm::cast<llvm::StructType>(GetType()->ToLLVM(env));

  for (const auto &member : members) {
    Outcome<llvm::Value *, Error> initializer_result = member->Codegen(env);
    if (!initializer_result) {
      return initializer_result;
    }

    if (auto *initializer =
            llvm::dyn_cast<llvm::Constant>(initializer_result.GetFirst())) {
      tuple_elements.push_back(initializer);
    } else {
      std::string errmsg =
          std::string("value is: ") + LLVMValueToString(initializer);
      return {Error(Error::Code::NonConstTupleInit, GetLoc(), errmsg)};
    }
  }

  return {llvm::ConstantStruct::get(tuple_type, tuple_elements)};
}
} // namespace pink
