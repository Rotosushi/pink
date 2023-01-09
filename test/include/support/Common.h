#pragma once
#include <ostream>

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
GetAstOrNull(pink::Outcome<std::unique_ptr<pink::Ast>, pink::Error> &outcome,
             std::ostream &out) -> pink::Ast * {
  if (outcome) {
    return outcome.GetFirst().get();
  }

  out << outcome.GetSecond().ToString("");
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
inline auto TestFnBodyIsA(pink::Ast *expr, pink::Location &loc,
                          std::ostream &out) -> bool {
  assert(expr != nullptr);
  pink::Function *fun = nullptr;
  pink::Block *body = nullptr;
  pink::Ast *body_expr = nullptr;

  bool result = (fun = llvm::dyn_cast<pink::Function>(expr)) != nullptr;
  if (!result) {
    out << "Expression was not a Function, was: " << expr->ToString() << "\n";
    return result;
  }

  result &= loc == fun->GetLoc();
  if (!result) {
    out << "Expected location: " << loc
        << " Function had location: " << fun->GetLoc() << "\n";
    return result;
  }

  result &= (body = llvm::dyn_cast<pink::Block>(fun->body.get())) != nullptr;
  if (!result) {
    out << "Expected function body to be a block, instead was: "
        << fun->body->ToString() << "\n";
    return result;
  }

  result &= body->begin() != body->end();
  if (!result) {
    out << "Function body is empty: " << fun->ToString() << "\n";
    return result;
  }

  result &= (body_expr = body->begin()->get()) != nullptr;
  if (!result) {
    out << "Function Body Expression was nullptr: " << fun->ToString() << "\n";
    return result;
  }

  result &= (llvm::isa<T>(body_expr));
  if (!result) {
    out << "Function body Expression was: " << body_expr->ToString();
    return result;
  }

  return result;
}

/*
   TestBindTerm exists to remove some boilerplate from testing if
   a bind's term 'isa' specific kind of ast.
*/
template <class T>
inline auto TestBindTermIsA(pink::Ast *expr, pink::Location &loc,
                            std::ostream &out) -> bool {
  assert(expr != nullptr);
  pink::Bind *bind = nullptr;
  bool result = (bind = llvm::dyn_cast<pink::Bind>(expr)) != nullptr;
  if (!result) {
    out << "Expression was not a bind expression: " << expr->ToString() << "\n";
    return result;
  }

  result = (loc == expr->GetLoc());
  if (!result) {
    out << "Expected Location: " << loc
        << " Bind has location: " << expr->GetLoc() << "\n";
    return result;
  }

  result = (llvm::isa<T>(bind->affix.get()));
  if (!result) {
    out << "Bind affix Expression was: " << bind->affix->ToString() << "\n";
    return result;
  }

  return result;
}

template <class T>
inline auto TestGetypeIsA(pink::Outcome<pink::Type *, pink::Error> &getype,
                          std::ostream &out) -> bool {
  bool result = (getype);
  if (!result) {
    out << getype.GetSecond().ToString("");
    return result;
  }

  result = (llvm::isa<T>(getype.GetFirst()));
  if (!result) {
    out << "Actual Type: " << getype.GetFirst()->ToString() << "\n";
    return result;
  }

  return result;
}
