#include "ast/Binop.h"

#include "aux/Environment.h"

#include "type/SliceType.h"

#include "visitor/AstVisitor.h"

namespace pink {
Binop::Binop(const Location &location, InternedString opr,
             std::unique_ptr<Ast> left, std::unique_ptr<Ast> right)
    : Ast(Ast::Kind::Binop, location), op(opr), left(std::move(left)),
      right(std::move(right)) {}

void Binop::Accept(AstVisitor *visitor) const { visitor->Visit(this); }

auto Binop::classof(const Ast *ast) -> bool {
  return ast->GetKind() == Ast::Kind::Binop;
}

auto Binop::ToString() const -> std::string {
  std::string result;
  if (llvm::isa<Binop>(left)) {
    result += "(" + left->ToString() + ")";
  } else {
    result += left->ToString();
  }

  result += " " + std::string(op) + " ";

  if (llvm::isa<Binop>(right)) {
    result += "(" + right->ToString() + ")";
  } else {
    result += right->ToString();
  }

  return result;
}

/*
    env |- lhs : Tl, rhs : Tr, op : Tl -> Tr -> T
  --------------------------------------------------
                      env |- lhs op rhs : T
*/
auto Binop::TypecheckV(const Environment &env) const -> Outcome<Type *, Error> {
  auto lhs_result = left->Typecheck(env);
  if (!lhs_result) {
    return lhs_result;
  }
  auto *lhs_type = lhs_result.GetFirst();

  auto rhs_result = right->Typecheck(env);
  if (!rhs_result) {
    return rhs_result;
  }
  auto *rhs_type = rhs_result.GetFirst();

  auto binop = env.binops->Lookup(op);

  // #RULE: any operator with zero overloads cannot be used in
  // an infix expression.
  if (!binop || binop->second->NumOverloads() == 0) {
    std::string errmsg = std::string("unknown op: ") + op;
    return {Error(Error::Code::UnknownBinop, GetLoc(), errmsg)};
  }

  // #RULE Binops are overloaded on their argument types
  auto literal = binop->second->Lookup(lhs_type, rhs_type);

  if (!literal) {
    std::string errmsg = "could not find an implementation of " +
                         std::string(op) +
                         " for given types: left: " + lhs_type->ToString() +
                         ", right: " + rhs_type->ToString();
    return {Error(Error::Code::ArgTypeMismatch, GetLoc(), errmsg)};
  }

  return {literal->second->result_type};
}

auto Binop::Codegen(const Environment &env) const
    -> Outcome<llvm::Value *, Error> {
  assert(GetType() != nullptr);
  // Get the type and value of both sides
  Outcome<Type *, Error> lhs_type_result(left->Typecheck(env));

  if (!lhs_type_result) {
    return {lhs_type_result.GetSecond()};
  }

  Outcome<llvm::Type *, Error> lhs_type_codegen =
      lhs_type_result.GetFirst()->ToLLVM(env);

  if (!lhs_type_codegen) {
    return {lhs_type_codegen.GetSecond()};
  }

  llvm::Type *lhs_type = lhs_type_codegen.GetFirst();

  Outcome<llvm::Value *, Error> lhs_value(left->Codegen(env));

  if (!lhs_value) {
    return lhs_value;
  }

  Outcome<Type *, Error> rhs_type_result(right->Typecheck(env));

  if (!rhs_type_result) {
    return {rhs_type_result.GetSecond()};
  }

  Outcome<llvm::Type *, Error> rhs_type_codegen =
      rhs_type_result.GetFirst()->ToLLVM(env);

  if (!rhs_type_codegen) {
    return {rhs_type_codegen.GetSecond()};
  }

  llvm::Type *rhs_type = rhs_type_codegen.GetFirst();

  Outcome<llvm::Value *, Error> rhs_value(right->Codegen(env));

  if (!rhs_value) {
    return rhs_value;
  }

  // find the operator present between both sides in the env
  llvm::Optional<std::pair<InternedString, BinopLiteral *>> binop =
      env.binops->Lookup(op);

  if (!binop) {
    std::string errmsg = std::string("unknown op: ") + op;
    return {Error(Error::Code::UnknownBinop, GetLoc(), errmsg)};
  }

  // find the instance of the operator given the type of both sides
  auto literal = binop->second->Lookup(lhs_type_result.GetFirst(),
                                       rhs_type_result.GetFirst());

  if (!literal) {
    std::string errmsg = std::string("could not find an implementation of ") +
                         std::string(op) + " for the given types, left: " +
                         lhs_type_result.GetFirst()->ToString() +
                         ", right: " + rhs_type_result.GetFirst()->ToString();
    return {Error(Error::Code::ArgTypeMismatch, GetLoc(), errmsg)};
  }

  // use the lhs and rhs values to generate the binop expression.
  return {literal->second->generate(lhs_type, lhs_value.GetFirst(), rhs_type,
                                    rhs_value.GetFirst(), env)};
}
} // namespace pink
