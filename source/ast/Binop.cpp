#include "ast/Binop.h"
#include "aux/Environment.h"

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
  // Get the type of both sides
  Outcome<Type *, Error> lhs_result(left->Typecheck(env));

  if (!lhs_result) {
    return lhs_result;
  }

  Outcome<Type *, Error> rhs_result(right->Typecheck(env));

  if (!rhs_result) {
    return rhs_result;
  }

  // find the operator present between both sides in the env
  llvm::Optional<std::pair<InternedString, BinopLiteral *>> binop =
      env.binops->Lookup(op);

  if (!binop || binop->second->NumOverloads() == 0) {
    std::string errmsg = std::string("unknown op: ") + op;
    return {Error(Error::Code::UnknownBinop, GetLoc(), errmsg)};
  }

  // #TODO: unop also needs this treatment for * and & operators.
  llvm::Optional<std::pair<std::pair<Type *, Type *>, BinopCodegen *>> literal;
  auto *pointer_type = llvm::dyn_cast<PointerType>(lhs_result.GetFirst());
  if ((pointer_type != nullptr) && (strcmp(op, "+") == 0)) {
    literal =
        binop->second->Lookup(lhs_result.GetFirst(), rhs_result.GetFirst());

    auto *int_type = llvm::dyn_cast<IntType>(rhs_result.GetFirst());
    if (int_type == nullptr) {
      std::string errmsg = std::string("lhs has pointer type: ") +
                           pointer_type->ToString() +
                           " however rhs is not an Int, rhs has type: " +
                           rhs_result.GetFirst()->ToString();
      return {Error(Error::Code::ArgTypeMismatch, GetLoc(), errmsg)};
    }

    if (!literal) {
      Type *int_ptr_type = env.types->GetPointerType(int_type);
      llvm::Optional<std::pair<std::pair<Type *, Type *>, BinopCodegen *>>
          ptr_add_binop = binop->second->Lookup(int_ptr_type, int_type);

      if (!ptr_add_binop) {
        FatalError("Couldn't find int ptr arithmetic binop!", __FILE__,
                   __LINE__);
      }

      BinopCodegenFn ptr_arith_fn = ptr_add_binop->second->generate;
      literal = binop->second->Register(lhs_result.GetFirst(), int_type,
                                        lhs_result.GetFirst(), ptr_arith_fn);
    }
  } else {
    // find the instance of the operator given the type of both sides
    literal =
        binop->second->Lookup(lhs_result.GetFirst(), rhs_result.GetFirst());
  }

  if (!literal) {
    std::string errmsg =
        std::string("could not find an implementation of ") + std::string(op) +
        " for given types, left: " + lhs_result.GetFirst()->ToString() +
        ", right: " + rhs_result.GetFirst()->ToString();
    return {Error(Error::Code::ArgTypeMismatch, GetLoc(), errmsg)};
  }

  // return the result type of calling the operator given the types of both
  // sides
  return {literal->second->result_type};
}

auto Binop::Codegen(const Environment &env) const
    -> Outcome<llvm::Value *, Error> {
  // Get the type and value of both sides
  Outcome<Type *, Error> lhs_type_result(left->Typecheck(env));

  if (!lhs_type_result) {
    return {lhs_type_result.GetSecond()};
  }

  Outcome<llvm::Type *, Error> lhs_type_codegen =
      lhs_type_result.GetFirst()->Codegen(env);

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
      rhs_type_result.GetFirst()->Codegen(env);

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

  llvm::Optional<std::pair<std::pair<Type *, Type *>, BinopCodegen *>> literal;
  auto *pointer_type = llvm::dyn_cast<PointerType>(lhs_type_result.GetFirst());
  // if this is a pointer addition operation, we must pass the pointee_type
  // into the binop code generation function for the GEP instruction within.
  // we can also safely add a new implementation of pointer addition no
  // matter what the underlying type is, because the GEP instruction
  // handles the offset computation based upon the pointee_type itself.
  // it is only the structure of the BinopLiteral itself that is going to
  // fail to find an implementation for any arbitrary array type we can
  // define.
  if ((pointer_type != nullptr) && (strcmp(op, "+") == 0)) {
    Outcome<llvm::Type *, Error> pointee_type_result =
        pointer_type->pointee_type->Codegen(env);

    if (!pointee_type_result) {
      return {pointee_type_result.GetSecond()};
    }

    lhs_type = pointee_type_result.GetFirst();

    auto *int_type = llvm::dyn_cast<IntType>(rhs_type_result.GetFirst());
    if (int_type == nullptr) {
      std::string errmsg = std::string("lhs has pointer type: ") +
                           pointer_type->ToString() +
                           " however rhs is not an Int, rhs has type: " +
                           rhs_type_result.GetFirst()->ToString();
      return {Error(Error::Code::ArgTypeMismatch, GetLoc(), errmsg)};
    }

    literal = binop->second->Lookup(lhs_type_result.GetFirst(), int_type);

    if (!literal) {
      Type *int_ptr_type = env.types->GetPointerType(env.types->GetIntType());
      llvm::Optional<std::pair<std::pair<Type *, Type *>, BinopCodegen *>>
          ptr_add_binop = binop->second->Lookup(int_ptr_type, int_type);

      if (!ptr_add_binop) {
        FatalError("Couldn't find int ptr arithmetic binop!", __FILE__,
                   __LINE__);
      }

      BinopCodegenFn ptr_arith_fn = ptr_add_binop->second->generate;
      literal =
          binop->second->Register(lhs_type_result.GetFirst(), int_type,
                                  lhs_type_result.GetFirst(), ptr_arith_fn);
    }
  } else {
    // find the instance of the operator given the type of both sides
    literal = binop->second->Lookup(lhs_type_result.GetFirst(),
                                    rhs_type_result.GetFirst());
  }

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
