#pragma once
#include "ast/Ast.h"

#include "ast/Bind.h"
#include "ast/Block.h"
#include "ast/Function.h"

/**
 * @brief Get the Ast from the Outcome, or return nullptr
 *
 * @param outcome
 * @return pink::Ast*
 */
inline auto
GetAstOrNull(pink::Outcome<std::unique_ptr<pink::Ast>, pink::Error> &outcome)
    -> pink::Ast * {
  if (outcome) {
    return outcome.GetFirst().get();
  }
  return nullptr;
}

/*
   TestFnBody exists to remove some boilerplate from testing if
   the first term within a functions body 'isa' specific kind of
   ast.

   You can theoretically write another routine which checks the
   type of the next term within the body, and add another template
   parameter for the tested type of that term. and that works for
   any finite number of terms you want to test within the body.
   though the function call syntax gets a bit unwieldy.
   you could write a packed template type and automate testing
   the type of each term within a function body.
*/
template <class T>
auto TestFnBodyIsA(pink::Ast *expr, pink::Location &loc) -> bool {
  pink::Function *fun = nullptr;
  pink::Block *body = nullptr;
  pink::Ast *body_expr = nullptr;

  bool result =
      ((fun = llvm::dyn_cast_or_null<pink::Function>(expr)) != nullptr) &&
      (loc == fun->GetLoc()) &&
      ((body = llvm::dyn_cast<pink::Block>(fun->body.get())) != nullptr) &&
      (body->begin() != body->end()) &&
      ((body_expr = body->begin()->get()) != nullptr) &&
      (llvm::isa<T>(body_expr));
  return result;
}

/*
   TestBindTerm exists to remove some boilerplate from testing if
   a bind's term 'isa' specific kind of ast.
*/
template <class T>
auto TestBindTermIsA(pink::Ast *expr, pink::Location &loc) -> bool {
  pink::Bind *bind = nullptr;
  bool result =
      ((bind = llvm::dyn_cast_or_null<pink::Bind>(expr)) != nullptr) &&
      (loc == expr->GetLoc()) && (llvm::isa<T>(bind->affix.get()));
  return result;
}

template <class T>
auto TestGetypeIsA(pink::Outcome<pink::Type *, pink::Error> &getype) -> bool {
  bool result = (getype) && (llvm::isa<T>(getype.GetFirst()));
  return result;
}
