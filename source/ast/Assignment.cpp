#include "ast/Assignment.h"
#include "aux/Environment.h"

#include "kernel/StoreAggregate.h"

#include "support/LLVMTypeToString.h"
#include "support/LLVMValueToString.h"

namespace pink {
Assignment::Assignment(const Location &location, std::unique_ptr<Ast> left,
                       std::unique_ptr<Ast> right)
    : Ast(Ast::Kind::Assignment, location), left(std::move(left)),
      right(std::move(right)) {}

auto Assignment::classof(const Ast *ast) -> bool {
  return ast->GetKind() == Ast::Kind::Assignment;
}

auto Assignment::ToString() const -> std::string {
  std::string result = left->ToString() + " = " + right->ToString();
  return result;
}

/*
    env |- lhs : Tl, rhs : Tr, Tl == Tr, lhs is-assignable
  ---------------------------------------------------------
                              env |- lhs = rhs : Tl

    note: When we implement local and global variables, the only
            llvm::Value*'s that are going to be bound in the symboltable
            are going to be pointers to places that can be assigned,
            either llvm::AllocaInsts*, or llvm::GlobalVariable*
*/
auto Assignment::TypecheckV(const Environment &env) const
    -> Outcome<Type *, Error> {
  // make sure we can type both sides
  Outcome<Type *, Error> lhs_type(left->Typecheck(env));

  if (!lhs_type) {
    return lhs_type;
  }

  Outcome<Type *, Error> rhs_type(right->Typecheck(env));

  if (!rhs_type) {
    return rhs_type;
  }

  // make sure the left and right hand sides are the same type
  if (lhs_type.GetFirst() != rhs_type.GetFirst()) {
    std::string errmsg = std::string("storage type: ") +
                         lhs_type.GetFirst()->ToString() +
                         ", value type: " + rhs_type.GetFirst()->ToString();
    Error error(Error::Code::AssigneeTypeMismatch, GetLoc(), errmsg);
    return {error};
  }
  // the types are equivalent, so it's immaterial which one we return.
  return {lhs_type.GetFirst()};
}

auto Assignment::Codegen(const Environment &env) const
    -> Outcome<llvm::Value *, Error> {
  // get the type and value of both sides
  Outcome<Type *, Error> lhs_type_result(left->Typecheck(env));

  if (!lhs_type_result) {
    return {lhs_type_result.GetSecond()};
  }

  Outcome<llvm::Type *, Error> lhs_type =
      lhs_type_result.GetFirst()->Codegen(env);

  if (!lhs_type) {
    return {lhs_type.GetSecond()};
  }

  env.flags->OnTheLHSOfAssignment(true);

  Outcome<llvm::Value *, Error> lhs_value(left->Codegen(env));

  env.flags->OnTheLHSOfAssignment(false);

  if (!lhs_value) {
    return lhs_value;
  }

  Outcome<Type *, Error> rhs_type_result(right->Typecheck(env));

  if (!rhs_type_result) {
    return {rhs_type_result.GetSecond()};
  }

  Outcome<llvm::Type *, Error> rhs_type =
      rhs_type_result.GetFirst()->Codegen(env);

  if (!rhs_type) {
    return {rhs_type.GetSecond()};
  }
  Outcome<llvm::Value *, Error> rhs_value(right->Codegen(env));

  if (!rhs_value) {
    return rhs_value;
  }

  // make sure the left and right hand sides are the same type
  if (lhs_type.GetFirst() != rhs_type.GetFirst()) {
    std::string errmsg =
        std::string("storage type: ") + LLVMTypeToString(lhs_type.GetFirst()) +
        ", value type: " + LLVMTypeToString(rhs_type.GetFirst());
    Error error(Error::Code::AssigneeTypeMismatch, GetLoc(), errmsg);
    return {error};
  }
  // okay, so how do we assign pointer values to pointer types,
  // and regular values to regular types,
  //
  // we have a few operations which are critical to think about,
  // variables
  // assignment
  // binding
  // AddressOfValue
  // ValueOfAddress
  //
  // llvm has been specifically designed to not need an address of
  // operator, in that the load and store instructions themselves
  // take the address of the memory being read/written.
  //
  // so, on the surface, this is easy, when we want to create a load
  // instruction, the place where we are writing can still be accessed
  // via a pointer, and the value can be either a regular value or
  // a pointer value. as long as the location is the correct type, the
  // load instruction will compiler. similarly for the store instruction,
  // the memory location being referenced will still be a pointer, and
  // the value will still be either a regular value or a pointer value,
  // and as long as the memory location being written to is the correct
  // type, the store instruction will compile.
  //
  // however, how our language currently operates is that the Variable
  // object itself constructs the load, and thus when a variable is
  // accessed, we retrieve a regular value and not the address of the
  // variable.
  // the only way we could ever take the address of a variable by
  // composing together the Variable ast node and the unop ast node
  // is if we remove that behavior from Variables.
  // however we still need to tell the difference between
  // x = y
  // and
  // x = &y
  //
  // and under the above rules, both rhs of the assignment expression
  // will return a llvm::Value which is the address of the memory
  // location of y. so, then we need to tell the difference within the
  // assignment expression itself. and we can actually do that, because
  // the operator will change the pink::Type* of the resulting
  // expression  from a simple type, to a pointer type.
  // and happily this is only in the case where the operator appears.
  //
  // however, this doesn't truly change the type of the llvm::Value
  // in either case. and that is important, because we don't want
  // that to change, we want it to be the case that assignment itself
  // is deciding wether or not to load from the address given before
  // performing the store associated with the assignment,
  // or simply use the address directly as the value being stored.
  //
  // we must be careful however, to note that if we codegen
  // a pink::PointerType that is pointing to a integer type,
  // well, since llvm gave us that integer type as a pointer
  // to an integer type, pink::PointerType will wrap that pointer type
  // in another llvm::PointerType, incorrectly returning a 2 star
  // pointer type..

  // we have the correct types, however we cannot assign to a literal value
  // here, only to a pointer, pointing to a valid memory location in the modules
  // global space or the local stack frame, so we check that the bound value is
  // able to be assigned to.
  if (llvm::isa<llvm::StructType>(lhs_type.GetFirst())) {
    llvm::Value *right_value = rhs_value.GetFirst();

    StoreAggregate(lhs_type.GetFirst(), lhs_value.GetFirst(),
                   rhs_value.GetFirst(), env);

    return {right_value};
  }

  if (llvm::isa<llvm::AllocaInst>(lhs_value.GetFirst()) ||
      (llvm::isa<llvm::GlobalVariable>(lhs_value.GetFirst())) ||
      (lhs_value.GetFirst()->getType()->getTypeID() ==
       llvm::Type::TypeID::PointerTyID)) {
    llvm::Value *right_value = rhs_value.GetFirst();

    env.instruction_builder->CreateStore(right_value, lhs_value.GetFirst());

    // return the value of the right hand side as the result to support nesting
    // assignment
    return {right_value};
  }

  std::string errmsg =
      std::string("llvm::Value* cannot be written: Value* is -> ") +
      LLVMValueToString(lhs_value.GetFirst());
  Error error(Error::Code::ValueCannotBeAssigned, GetLoc(), errmsg);
  return {error};
}
} // namespace pink
