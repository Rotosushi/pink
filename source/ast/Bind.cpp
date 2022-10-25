
#include "ast/Bind.h"

#include "aux/Environment.h"
#include "kernel/StoreAggregate.h"

#include "visitor/AstVisitor.h"

#include "llvm/IR/GlobalVariable.h"

namespace pink {
Bind::Bind(const Location &location, InternedString symbol,
           std::unique_ptr<Ast> affix)
    : Ast(Ast::Kind::Bind, location), symbol(symbol), affix(std::move(affix)) {}

void Bind::Accept(AstVisitor *visitor) const { visitor->Visit(this); }

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
  /**
   * \todo add namespaces (maybe in the form of modules) to the language.
   * namespaces are super useful.
   *
   */

  // #RULE: Bind only checks for symbol Redefinition in the Local Scope
  auto bound = env.bindings->LookupLocal(symbol);

  if (!bound.has_value()) {
    Outcome<Type *, Error> affix_type_result = affix->Typecheck(env);

    if (!affix_type_result) {
      return affix_type_result;
    }

    auto *affix_type = affix_type_result.GetFirst();

    // construct a false binding so we can properly
    // type statements that occur later within the same block.
    env.false_bindings->push_back(symbol);
    env.bindings->Bind(symbol, affix_type, nullptr);

    return {affix_type};
  }

  std::string errmsg =
      std::string(symbol) + " has type: " + bound->first->ToString();
  return {Error(Error::Code::NameAlreadyBoundInScope, GetLoc(), errmsg)};
}

auto Bind::Codegen(const Environment &env) const
    -> Outcome<llvm::Value *, Error> {
  auto bound = env.bindings->LookupLocal(symbol);

  auto *term_type = affix->GetType();
  assert(term_type != nullptr);

  if (!bound.has_value()) {
    llvm::Type *llvm_term_type = term_type->Codegen(env);

    auto term_value_result = affix->Codegen(env);
    if (!term_value_result) {
      return term_value_result;
    }
    llvm::Value *term_value = term_value_result.GetFirst();
    assert(term_value != nullptr);

    llvm::Value *ptr = nullptr;
    // this is the global scope, construct a global
    if (env.current_function == nullptr) {
      ptr = env.llvm_module->getOrInsertGlobal(symbol, llvm_term_type);

      llvm::GlobalVariable *global = env.llvm_module->getGlobalVariable(symbol);
      // Rule: Globals must have Constant initializers
      auto *constant = llvm::dyn_cast<llvm::Constant>(term_value);
      if (constant != nullptr) {
        global->setInitializer(constant);
      } else {
        return {Error(Error::Code::NonConstGlobalInit, affix->GetLoc())};
      }
    } else { // this is a local scope, so construct a local binding
      auto *insertion_block = &(env.current_function->getEntryBlock());
      auto insertion_point = insertion_block->getFirstInsertionPt();
      llvm::IRBuilder local_builder(insertion_block, insertion_point);

      if (llvm_term_type->isSingleValueType()) {
        ptr = local_builder.CreateAlloca(llvm_term_type,
                                         env.data_layout.getAllocaAddrSpace(),
                                         nullptr, symbol);

        env.instruction_builder->CreateStore(term_value, ptr);

      } else if (auto *struct_type =
                     llvm::dyn_cast<llvm::StructType>(llvm_term_type)) {
        ptr = local_builder.CreateAlloca(
            struct_type, env.data_layout.getAllocaAddrSpace(), nullptr, symbol);

        StoreAggregate(struct_type, ptr, term_value, env);
      } else {
        // This is a location where we cannot replace FatalError with assert.
        FatalError("Unsupported Type* on RHS of Bind", __FILE__, __LINE__);
      }
    }
    // bind the symbol to the newly created value
    env.bindings->Bind(symbol, term_type, ptr);

    // Rule: Bind is an Expression
    return {term_value};
  }
  // #PROPOSAL: if the rhs term has the same type as what the symbol
  //  is already bound to, we could treat the binding as equivalent
  //  to assignment here.
  std::string errmsg = std::string(symbol) +
                       " already bound to type: " + bound->first->ToString();
  return {Error(Error::Code::NameAlreadyBoundInScope, GetLoc(), errmsg)};
}
} // namespace pink
