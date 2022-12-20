#include "ast/Unop.h"
#include "aux/Environment.h"

#include "visitor/AstVisitor.h"

namespace pink {
Unop::Unop(const Location &location, InternedString opr,
           std::unique_ptr<Ast> right)
    : Ast(Ast::Kind::Unop, location), op(opr), right(std::move(right)) {}

void Unop::Accept(const ConstAstVisitor *visitor) const {
  visitor->Visit(this);
}

auto Unop::classof(const Ast *ast) -> bool {
  return ast->GetKind() == Ast::Kind::Unop;
}

auto Unop::ToString() const -> std::string { return op + right->ToString(); }

/*
        env |- rhs : Tr, op : Tr -> T
  ----------------------------------------
                    env |- op rhs : T
*/
auto Unop::TypecheckV(const Environment &env) const -> Outcome<Type *, Error> {
  // get the type of the rhs
  auto rhs_result = right->Typecheck(env);
  if (!rhs_result) {
    return rhs_result;
  }

  // find the operator used in the env
  auto unop = env.unops->Lookup(op);

  if (!unop) {
    std::string errmsg = std::string("unknown unop: ") + op;
    return {Error(Error::Code::UnknownUnop, GetLoc(), errmsg)};
  }

  // find the overload of the operator for the given type
  llvm::Optional<std::pair<Type *, UnopCodegen *>> literal;
  auto *pointer_type = llvm::dyn_cast<PointerType>(rhs_result.GetFirst());
  if ((pointer_type != nullptr) && (strcmp(op, "*") == 0)) {
    literal = unop->second->Lookup(rhs_result.GetFirst());

    if (!literal) {
      Type *int_ptr_type = env.types->GetPointerType(env.types->GetIntType());
      auto ptr_indirection_unop = unop->second->Lookup(int_ptr_type);

      if (!ptr_indirection_unop) {
        FatalError("Couldn't find int ptr indirection unop!", __FILE__,
                   __LINE__);
      }

      UnopCodegenFn ptr_indirection_fn = ptr_indirection_unop->second->generate;
      literal = unop->second->Register(pointer_type, pointer_type->pointee_type,
                                       ptr_indirection_fn);
    }
  } else if (strcmp(op, "&") == 0) {
    literal = unop->second->Lookup(rhs_result.GetFirst());

    if (!literal) {
      PointerType *pointer_type =
          env.types->GetPointerType(rhs_result.GetFirst());
      Type *int_type = env.types->GetIntType();
      llvm::Optional<std::pair<Type *, UnopCodegen *>> address_of_unop =
          unop->second->Lookup(int_type);

      if (!address_of_unop) {
        FatalError("Couldn't find int address_of unop!", __FILE__, __LINE__);
      }

      UnopCodegenFn address_of_fn = address_of_unop->second->generate;
      literal = unop->second->Register(rhs_result.GetFirst(), pointer_type,
                                       address_of_fn);
    }
  } else {
    literal = unop->second->Lookup(rhs_result.GetFirst());
  }

  if (!literal) {
    std::string errmsg =
        "could not find an implementation of " + std::string(op) +
        " for the given type: " + rhs_result.GetFirst()->ToString();
    return {Error(Error::Code::ArgTypeMismatch, GetLoc(), errmsg)};
  }

  // return the result type of applying the operator to the given type
  return {literal->second->result_type};
}

/*

        env |- rhs : Tr, op : Tr -> T
  ----------------------------------------
                    env |- op rhs : unop.generate(rhs.value)

*/
auto Unop::Codegen(const Environment &env) const
    -> Outcome<llvm::Value *, Error> {
  // get the type of the right hand side for operator lookup
  Outcome<Type *, Error> rhs_type(right->Typecheck(env));

  if (!rhs_type) {
    return {rhs_type.GetSecond()};
  }

  // get the value to generate the llvm ir

  Outcome<llvm::Value *, Error> rhs_value(Error(Error::Code::None, Location()));

  if (strcmp(op, "&") == 0) {
    // we cannot assign to an address value, we can only assign to
    // a memory location.
    if (env.flags->OnTheLHSOfAssignment()) {
      return {Error(Error::Code::ValueCannotBeAssigned, GetLoc())};
    }

    // take the address of the rhs,
    //
    // all setting this flag does is stop Variable::Codegen
    // from emitting a load instruction on the pointer to
    // memory the Variable is bound to.
    // I am fairly sure this is the only thing we can take
    // address of, but this is a place where we might see unexpected
    // behavior as a result of this descision.
    env.flags->WithinAddressOf(true);

    rhs_value = right->Codegen(env);

    env.flags->WithinAddressOf(false);
  } else if (strcmp(op, "*") == 0) {
    // dereference the rhs
    // if we are on the lhs of an assignment expression
    // only emit one load, else we want to load the
    // value from the pointer that we retrieved from
    // loading the pointer the variable was bound to.
    if (env.flags->OnTheLHSOfAssignment()) {
      // emit one load. which Variable::Codegen already does for us,
      // so we just have to pretend we aren't on the lhs of assignment
      env.flags->OnTheLHSOfAssignment(false);
      env.flags->WithinDereferencePtr(true);

      rhs_value = right->Codegen(env);

      env.flags->OnTheLHSOfAssignment(true);
      env.flags->WithinDereferencePtr(false);
    } else {
      // emit two loads, so we just have to emit a load on the llvm::Value*
      // Codegening the rhs returned, so tell Variable::Codegen that we
      // want to dereference the pointer it has
      env.flags->WithinDereferencePtr(true);

      rhs_value = right->Codegen(env);

      env.flags->WithinDereferencePtr(false);

      if (!rhs_value) {
        return rhs_value;
      }

      // strip off the ptr type so we can construct a load of the value
      // within the ptr.
      // we know that the Type of the rhs is a PointerType, otherwise the
      // typechecker would have never typed this expression.
      auto *ptr_type = llvm::cast<pink::PointerType>(rhs_type.GetFirst());

      Outcome<llvm::Type *, Error> llvm_pointee_type =
          ptr_type->pointee_type->ToLLVM(env);

      if (!llvm_pointee_type) {
        return {llvm_pointee_type.GetSecond()};
      }

      if (llvm_pointee_type.GetFirst()->isSingleValueType()) {
        rhs_value = env.instruction_builder->CreateLoad(
            llvm_pointee_type.GetFirst(), rhs_value.GetFirst(), "deref");
      }
      // else emit no load for types that cannot be loaded
    }
  } else // just emit the rhs as we normally do for unops
  {
    rhs_value = right->Codegen(env);

    if (!rhs_value) {
      return rhs_value;
    }
  }

  // find the literal
  llvm::Optional<std::pair<InternedString, UnopLiteral *>> unop =
      env.unops->Lookup(op);

  if (!unop) {
    return {Error(Error::Code::UnknownUnop, GetLoc(), op)};
  }

  // find the overload of the operator for the given type
  llvm::Optional<std::pair<Type *, UnopCodegen *>> literal =
      unop->second->Lookup(rhs_type.GetFirst());

  if (!literal) {
    return {Error(Error::Code::ArgTypeMismatch, GetLoc(),
                  rhs_type.GetFirst()->ToString())};
  }

  // use the operators associated generator expression to generate the llvm ir
  Outcome<llvm::Value *, Error> unop_value(
      literal->second->generate(rhs_value.GetFirst(), env));
  // either outcome, return the result.
  return {unop_value};
}
} // namespace pink
