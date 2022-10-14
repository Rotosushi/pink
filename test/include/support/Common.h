#pragma once
#include "ast/Ast.h"

#include "ast/Bind.h"
#include "ast/Block.h"
#include "ast/Function.h"

/*
   TestFnBody exists to remove some boilerplate from testing if
   the first term within a functions body 'isa' specific kind of
   ast.

   You can theoretically write another routine which checks the
   type of the next term within the body, and add another template
   parameter for the tested type of that term. and that works for
   any finite number of terms you want to test within the body.
   though the function call syntax gets a bit unwieldy.
*/

template <class T>
auto TestFnBodyIsA(pink::Ast *expr, pink::Location &loc) -> bool {
  const pink::Function *fun = nullptr;
  const pink::Block *body = nullptr;
  const pink::Ast *body_expr = nullptr;

  bool result =
      ((fun = llvm::dyn_cast<pink::Function>(expr)) != nullptr) &&
      (loc == fun->GetLoc()) &&
      ((body = llvm::dyn_cast<pink::Block>(fun->GetBody())) != nullptr) &&
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

  bool result = ((bind = llvm::dyn_cast<pink::Bind>(expr)) != nullptr) &&
                (loc == expr->GetLoc()) && (llvm::isa<T>(bind->GetAffix()));
  return result;
}

template <class T>
auto TestGetypeIsA(pink::Outcome<pink::Type *, pink::Error> &getype) -> bool {
  bool result = (getype) && (llvm::isa<T>(getype.GetFirst()));
  return result;
}

auto GetAstOrNull(
    pink::Outcome<std::unique_ptr<pink::Ast>, pink::Error> &outcome)
    -> pink::Ast *;
