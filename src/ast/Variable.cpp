#include "ast/Variable.h"
#include "aux/Environment.h"

namespace pink {
Variable::Variable(const Location &location, InternedString symbol)
    : Ast(Ast::Kind::Variable, location), symbol(symbol) {}

auto Variable::classof(const Ast *ast) -> bool {
  return ast->GetKind() == Ast::Kind::Variable;
}

auto Variable::ToString() const -> std::string { return {symbol}; }

/*
        env |- x is-in env, x : T
  -----------------------------
                  env |- x : T
*/
auto Variable::GetypeV(const Environment &env) const -> Outcome<Type *, Error> {
  auto bound = env.bindings->Lookup(symbol);

  if (bound.has_value()) {
    return {bound->first};
  }

  std::string errmsg = std::string("unknown symbol: ") + symbol;
  return {Error(Error::Code::NameNotBoundInScope, GetLoc(), errmsg)};
}

auto Variable::Codegen(const Environment &env) const
    -> Outcome<llvm::Value *, Error> {
  auto bound = env.bindings->Lookup(symbol);

  if (bound.has_value()) {
    assert(bound->second != nullptr);

    auto type_result = bound->first->Codegen(env);

    if (!type_result) {
      return {type_result.GetSecond()};
    }

    auto *bound_type = type_result.GetFirst();

    // llvm gives us pointers to the memory it allocates, this is fine,
    // as it gives us the means to assign new values and retrieve the
    // value stored within.
    //
    // There are two cases that we have to handle with non-pointer typed
    // variables within our language
    // 1) assigning a new value "x = ...;"
    // 2) retrieving the value  "... = x;" or "... = x + ...;"
    //
    // 1) when we are assigning a new value to the variable,
    //    we want to return the pointer llvm gave us as the
    //    result of this Variable::Codegen, so we can pass that
    //    pointer in as the argument to the CreateStore instruction.
    //
    // 2) when we are simply using the variable within an expression
    //    anywhere other than the lhs of assignment we want to load
    //    the value of the variable and return that so the value
    //    can be used as the argument to arithmetic operations or
    //    bound to a function argument for a Call expression.
    //
    //  this behavior is easy to choose between as we require only
    //  one bit of information, telling us if we are on the left
    //  hand side of an assignment expression, if we -are- on the
    //  left, we emit zero loads, if we aren't we emit one load.
    //
    // There are similarly only two cases we need to consider when
    // taking the address of a variable in our language.
    //
    // 1) "x = &y;"
    // 2) "x = &y op ...;"
    // 3) "&x = ...;"
    //
    // 1) we want to return the pointer that llvm gave us as the
    //    value of the variable reference. essentially the
    //    AddressOf operator simply has to stop Variable::Codegen
    //    from emitting a load instruction
    //
    // 2) We want to return the pointer that llvm gave us as the
    //    value of the variable reference, just like case 1.
    //
    // 3) is an error, we cannot assign to a literal integer, which is
    //    what the pointer in-and-of-itself is.
    //
    // So there are four cases that we have to handle to
    // properly support dereferencing pointer typed
    // variables within our language.
    // 1) "x = &y;"  assigning a new pointer
    // 2) "*x = y;" assigning a new value through the pointer
    // 3) "y = x;" or "y = x + z;"   retrieving the pointer as a value
    // 4) "y = *x;" or "y = *x + z;" retrieving the value through the pointer
    //
    // 1) when we are on the lhs of the assignment instruction,
    //    and we are not dereferencing the pointer, we want the
    //    result to be simply the pointer that llvm gave us to the
    //    memory location holding the pointer itself. that way we
    //    can reassign the pointer to a different pointer value
    //    via a CreateStore instruction.
    //    this means we want to emit zero CreateLoad instructions
    //    A -> B -> Value
    //    return A
    //
    //
    // 2) when we want to assign a new value through the pointer,
    //    we need to dereference the pointer itself, and return the
    //    pointer to the memory location we were originally pointing
    //    to so that can be used as an argument to a CreateStore
    //    instruction, which itself take a pointer to the memory it
    //    modifies
    //    A -> B -> Value
    //    return B
    //
    // 3) When we want to use the pointer itself as a value within an
    // expression
    //    we have to retrieve that value, which means we need to emit one
    //    CreateLoad Instruction. just like we do with regular variables to
    //    their value.
    //    A -> B -> Value
    //    return B
    //
    // 4) When we want to use the value pointed to by the pointer within an
    // expression
    //    and we are derefrencing the pointer itself we want to emit two
    //    CreateLoad Instructions
    //    A -> B -> Value
    //    return Value
    //
    //  we need two bits of information to solve this problem, so lets set
    //  up a truth table.
    //
    //  LHSOf= | Deref | Load?
    //    0    |   0   | Load once to get the ptr to the value (case 3)
    //    0    |   1   | Load twice to get the value (case 4)
    //    1    |   0   | Dont Load at all (case 1)
    //    1    |   1   | Load once to get the ptr to the value (case 2)
    //
    // so, we can break this down into fewer cases if we consider that 3
    // and 2 require the same behavior, so in this way, we could reframe
    // this as:
    //
    //  A) if we simply encounter a variable reference this generates
    //    a LoadInstruction to turn the ptr llvm gives us into it's
    //    corresponding value.
    //    this handles case 3, because variable references on the rhs
    //    of assignment are treated the same as within arguments to
    //    functions.
    //
    //  B) when we are emitting code on the lhs of an assignment expression
    //    we want to suppress the generation of Load instructions, such that
    //    we can use the rhs llvm::Value* as a pointer to a memory location
    //    that can be assigned.
    //
    //  C) when we take the address of a variable we simply return the pointer
    //    llvm Gave us, which means suppressing a load of a variable
    //    reference that presumably occurs on the rhs of the operator.
    //    the operator must return a pointer to a valid memory location for
    //    further load/stores.
    //
    //  D) when we dereference a pointer, we must generate an additional
    //    load instruction, on top of the one that Variable::Codegen
    //    emits, this is because the memory location we have a pointer
    //    to itself holds the pointer to the data.
    //
    //  E) if we dereference a ptr on the lhs of assignment, we want to
    //    suppress the emission of only one of the load instructions.
    //    this will give the CreateStore instruction which implements
    //    assignment a pointer to the memory it modifies.
    //
    //
    //
    // We dont emit a load instruction in any of these cases.
    if (llvm::isa<llvm::FunctionType>(bound_type) ||
        (llvm::isa<llvm::ArrayType>(bound_type)) ||
        (llvm::isa<llvm::StructType>(bound_type)) ||
        env.flags->OnTheLHSOfAssignment() || env.flags->WithinAddressOf()) {
      return {bound->second};
    }

    return {
        env.instruction_builder->CreateLoad(bound_type, bound->second, symbol)};
  }

  std::string errmsg = std::string("unknown symbol: ") + symbol;
  return {Error(Error::Code::NameNotBoundInScope, GetLoc(), errmsg)};
}
} // namespace pink
