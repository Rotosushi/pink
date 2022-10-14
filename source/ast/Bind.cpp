
#include "ast/Bind.h"

#include "aux/Environment.h"
#include "kernel/StoreAggregate.h"

#include "visitor/AstVisitor.h"

#include "llvm/IR/GlobalVariable.h"

namespace pink {
Bind::Bind(const Location &location, InternedString symbol,
           std::unique_ptr<Ast> affix)
    : Ast(Ast::Kind::Bind, location), symbol(symbol), affix(std::move(affix)) {}

void Bind::Accept(AstVisitor *visitor) { visitor->Visit(this); }

auto Bind::classof(const Ast *ast) -> bool {
  return ast->GetKind() == Ast::Kind::Bind;
}

auto Bind::ToString() const -> std::string {
  return std::string(symbol) += " := " + affix->ToString();
}

/*
    env |- t : T, x is-not-in env.bindings
    ---------------------------------------
              env |- x := t : T, x : T

    note: Typeing a binding expression is distinct
            from evaluating it. and it is only when we
            have both type and value that we can construct
            a binding in the symboltable.
*/
auto Bind::TypecheckV(const Environment &env) const -> Outcome<Type *, Error> {
  /*
           for the case of a binding statement, which declares a new variable,
           we only want to lookup the symbol in the local scope. because then
           we can construct shadowed names. which makes it easier to write
           functions with local variables, because we can reuse the names
           already bound in higher scopes, in the lowe scope.

           This is only because of the bind terms construction of a new binding.
           Variable references still do recursive lookup, in order to resolve
           the case where a function uses a global name within it's body.

           and this still prevents local and global redefinition of symbols,
           it only allows redefinition in the case where the local scope is
           defining a symbol that is defined in an outer scope.

           #TODO add namespace qualifiers into the language, which will then
                          allow local scopes to shadow a name and still have
     access to the outer name via the fully qualified name. The shadowing then
     will be allowed by the local bindings getting a different fully qualified
                          symbol than the global name would. all this would be
     transparent to the programmer, and would only be on a 'opt-in' basis.
  */
  llvm::Optional<std::pair<Type *, llvm::Value *>> bound(
      env.bindings->LookupLocal(symbol));

  if (!bound.has_value()) {
    Outcome<Type *, Error> affix_type = affix->Typecheck(env);

    if (affix_type) {
      if (auto *array_type = llvm::dyn_cast<ArrayType>(affix_type.GetFirst())) {
        // array's decompose into pointers to their first element.
        env.false_bindings->push_back(symbol);

        Type *ptr_type = env.types->GetPointerType(array_type->member_type);

        env.bindings->Bind(symbol, ptr_type, nullptr);
        return {ptr_type};
      }
      // construct a false binding so we can properly
      // type statements that occur later within the same block.
      env.false_bindings->push_back(symbol);

      env.bindings->Bind(symbol, affix_type.GetFirst(),
                         nullptr); // construct a false binding to type later
                                   // statements within this same block.

      return {affix_type.GetFirst()};
    }
    // affix type is actually an Error object
    return affix_type;
  }
  // The name is already bound to some type, so we cannot
  // type this bind expression.
  // note: if the binding term has the same type as what the symbol
  //  is already bound to, we could treat the binding as equivalent
  //  to an assignment instead of an error. different types would still be an
  //  error.
  std::string errmsg =
      std::string(symbol) + " has type: " + bound->first->ToString();
  return {Error(Error::Code::NameAlreadyBoundInScope, GetLoc(), errmsg)};
}

/*
    constructs a new variable with the name associated with the bind expression.
    if the current symboltable in the global scope, then we construct a global
    variable. once functions are added to the language then if the symboltable
    is associated with a function, i.e. is a local scope, then we allocate the
    new variable on the stack by constructing an alloca.

  okay, since this procedure is responsible for allocating space for new
  variables this is the location which needs to handle allocating aggregate
  types vs single value types.

  We cannot emit a store instruction for an aggregate type, only for single
  value types. thus, to initialize an array to some given set of values, we have
  to assign each of them individually.

  It makes sense to me that on the destination side, for each slot in the array
  we emit a GEP to construct a pointer to the slot, and then we emit a store of
  the given value for that slot.

  what is confusing to me is the role that llvm::ConstantArray plays here.
  do we simply emit a GEP to the slot in the ConstantArray and then emit a
  load of that slot, for each slot? essentially the inverse of what we do for
  the dest? that makes the most sense, except for the part of where does the
  constant array live in memory that we can validly construct a GEP?
  i dunno, only one way to see if it works, i just hope it doesn't segfault.
*/
auto Bind::Codegen(const Environment &env) const
    -> Outcome<llvm::Value *, Error> {
  llvm::Optional<std::pair<Type *, llvm::Value *>> bound(
      env.bindings->LookupLocal(symbol));

  if (!bound.has_value()) {
    // retrieve the type of the term
    Outcome<Type *, Error> term_type_result = affix->Typecheck(env);

    if (!term_type_result) {
      return {term_type_result.GetSecond()};
    }

    // get the llvm representation of the type
    Outcome<llvm::Type *, Error> term_type =
        term_type_result.GetFirst()->Codegen(env);

    if (!term_type) {
      return {term_type.GetSecond()};
    }

    // get the llvm representation of the term's value
    Outcome<llvm::Value *, Error> term_value_result = affix->Codegen(env);

    if (!term_value_result) {
      return term_value_result;
    }

    llvm::Value *term_value = term_value_result.GetFirst();
    // ptr is the thing we bind variables too, so it needs
    // declared in a greater scope than where we can initialize
    // it, to unify the result of the cases of decomposing
    // arrays to pointers, but nothing else.
    llvm::Value *ptr = nullptr;

    if (env.current_function ==
        nullptr) // this is the global scope, construct a global
    {
      // global_value is the address of the value in question, and has to be
      // retrieved directly from the module,
      //	which should be fine, as the ir_builder will also be associated
      // with the same module.
      // in the case of a global variable, the name is bound to the address of
      // the global variable's location constructed w.r.t. the module.
      ptr = env.module->getOrInsertGlobal(symbol, term_type.GetFirst());

      llvm::GlobalVariable *global = env.module->getGlobalVariable(symbol);

      if (llvm::isa<llvm::Constant>(term_value)) {
        auto *constant = llvm::dyn_cast<llvm::Constant>(term_value);
        global->setInitializer(constant);
      } else {
        return {Error(Error::Code::NonConstGlobalInit, affix->GetLoc())};
      }
    } else // this is a local scope, so construct a local binding
    {
      llvm::BasicBlock *insertion_block =
          &(env.current_function->getEntryBlock());
      llvm::BasicBlock::iterator insertion_point =
          insertion_block->getFirstInsertionPt();

      // create a new builder to not mess with the previous ones insertion
      // point. we might be able to save/restore the point, but what if the
      // previous builder was in the global scope, and thus had no basic block
      // to insert into?
      llvm::IRBuilder local_builder(insertion_block, insertion_point);

      // check if the value being stored fits within a register, a.k.a. single
      // value types, because then we only have to emit a single store
      // instruction to bind the value to the location we allocated. otherwise
      // we have to break apart the given type into peices which are single
      // values, and then we can emit a store instruction for each of those
      // peices, iterating over all single values within the given type will
      // store the entire thing.

      // we can call llvm::Type::isSingleValueType to check, and then emit
      // more than one store. however, to properly encompass the recursive
      // nature of types, i think we will need to define a recursive
      // proceudre to emit the correct sequence of Load instructions.
      // and we cannot simply use a loop here.
      llvm::Type *rhs_type = term_type.GetFirst();

      if (rhs_type->isSingleValueType()) {
        // in the case of a local symbol the name is bound to the address of
        // the local variables location, constructed w.r.t. the current
        // function.
        ptr = local_builder.CreateAlloca(
            rhs_type, env.data_layout.getAllocaAddrSpace(), nullptr, symbol);

        // we can store the value into the stack (alloca) at some
        // point after we construct it. in this case, relative to
        // where the bind declaration is located syntactically.
        env.instruction_builder->CreateStore(term_value, ptr);
      } else if (auto *array_type = llvm::dyn_cast<llvm::ArrayType>(rhs_type)) {
        //
        // llvm::ConstantArray* ca =
        // llvm::dyn_cast<llvm::ConstantArray>(term_value);

        // if (ca == nullptr)
        //{
        //   FatalError("term value was not a ConstantArray", __FILE__,
        //   __LINE__);
        // }

        ptr = local_builder.CreateAlloca(
            array_type, env.data_layout.getAllocaAddrSpace(),
            local_builder.getInt64(array_type->getNumElements()), symbol);
        // This function encapsulates the series of GEP and store
        // instructions needed to store a potentially recursive
        // aggregate type.
        // interestingly, using this procedure breaks arrays,
        // even though there is only a difference between using llvm::cast
        // up here, and llvm::dyn_cast within the function.
        // and this function breaks if we swap dyn_cast for cast,
        // even though when building from a constant array literal
        // we can step through the debugger and see that term->codegen(env)
        // is returning an instance of a llvm::ConstantArray...
        // so why is the dyn_cast returning nullptr if we can follow the
        // execution and see that the llvm::Value is in fact an instance of
        // an llvm::ConstantArray?
        StoreAggregate(array_type, ptr, term_value, env);

        // array types decompose to pointer types when referenced
        // by name within the program, so we bind the array name
        // to a pointer to the first element.
        // ptr = env->builder->CreateBitCast(ptr,
        // env->builder->getPtrTy(env->data_layout.getAllocaAddrSpace()),
        // "bcast");
        ptr =
            env.instruction_builder->CreateConstGEP2_64(array_type, ptr, 0, 0);
      } else if (auto *struct_type =
                     llvm::dyn_cast<llvm::StructType>(rhs_type)) {
        ptr = local_builder.CreateAlloca(
            struct_type, env.data_layout.getAllocaAddrSpace(), nullptr, symbol);

        StoreAggregate(struct_type, ptr, term_value, env);
      } else {
        FatalError("Unsupported Type* passed to Bind's allocator", __FILE__,
                   __LINE__);
      }
    }
    // we use term_type_result, as that holds a pink::Type*, which is what Bind
    // expects.
    env.bindings->Bind(symbol, term_type_result.GetFirst(),
                       ptr); // bind the symbol to the newly created value

    return {term_value}; // return the value of the right hand side to support
                         // nested binds.
  }
  // #TODO: if the binding term has the same type as what the symbol
  //  is already bound to, we could treat the binding as equivalent
  //  to an assignment here.
  std::string errmsg = std::string(symbol) +
                       " already bound to type: " + bound->first->ToString();
  return {Error(Error::Code::NameAlreadyBoundInScope, GetLoc(), errmsg)};
}
} // namespace pink
