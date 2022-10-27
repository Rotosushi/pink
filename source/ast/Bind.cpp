
#include "ast/Bind.h"

#include "aux/Environment.h"

#include "kernel/AllocateVariable.h"
#include "kernel/StoreValue.h"

#include "visitor/AstVisitor.h"

#include "support/LLVMValueToString.h"

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

/**
 * \todo add namespaces (maybe in the form of modules) to the language.
 * namespaces are super useful.
 *
 */

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
  // #RULE: Bind only checks for symbol redefinition in the Local Scope
  // this allows for shadow declarations
  auto bound = env.bindings->LookupLocal(symbol);

  if (bound.has_value()) {
    std::string errmsg =
        std::string(symbol) + " has type: " + bound->first->ToString();
    return {Error(Error::Code::NameAlreadyBoundInScope, GetLoc(), errmsg)};
  }

  auto affix_type_result = affix->Typecheck(env);
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

auto Bind::Codegen(const Environment &env) const
    -> Outcome<llvm::Value *, Error> {
  assert(GetType() != nullptr);
  auto bound = env.bindings->LookupLocal(symbol);

  if (bound.has_value()) {
    // #PROPOSAL: if the rhs term has the same type as what the symbol
    //  is already bound to, we could treat the binding as equivalent
    //  to assignment here.
    std::string errmsg = std::string(symbol) +
                         " already bound to type: " + bound->first->ToString();
    FatalError(errmsg, __FILE__, __LINE__);
  }

  auto *term_type = affix->GetType()->ToLLVM(env);
  assert(term_type != nullptr);

  auto term_value_result = affix->Codegen(env);
  if (!term_value_result) {
    return term_value_result;
  }
  llvm::Value *term_value = term_value_result.GetFirst();
  assert(term_value != nullptr);

  llvm::Value *ptr = AllocateVariable(symbol, term_type, env, term_value);

  // bind the symbol to the newly created value
  env.bindings->Bind(symbol, affix->GetType(), ptr);

  // Rule: Bind is an Expression
  return {term_value};
}
} // namespace pink
