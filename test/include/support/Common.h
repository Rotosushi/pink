#pragma once
#include <ostream>

#include "aux/Environment.h"

#include "ast/Ast.h"
#include "ast/Bind.h"
#include "ast/Block.h"
#include "ast/Conditional.h"
#include "ast/Function.h"
#include "ast/While.h"

#include "ast/action/ToString.h"

#include "type/action/ToString.h"

inline auto
GetAstOrNull(std::ostream                                   &out,
             pink::Outcome<pink::Ast::Pointer, pink::Error> &outcome,
             pink::Environment &env) -> pink::Ast::Pointer & {
  if (outcome) {
    return outcome.GetFirst();
  }

  env.PrintErrorWithSourceText(out, outcome.GetSecond());
  return nullptr;
}

template <class T>
inline auto TestBlocksFirstExpr(const pink::Ast::Pointer &expr,
                                std::ostream             &out) -> bool {
  auto *block = llvm::dyn_cast<pink::Block>(expr);

  bool result = (block == nullptr);
  if (!result) {
    out << "Expression was not a block [" << ToString(expr) << "]\n";
    return result;
  }

  result &= (block->begin() != block->end());
  if (!result) {
    out << "Block is empty [" << ToString(expr) << "]\n";
    return result;
  }

  auto *first = block->begin()->get();
  result      &= (first != nullptr);
  if (!result) {
    out << "Blocks first expression was empty.\n";
    return result;
  }

  result &= (llvm::isa<T>(first));
  if (!result) {
    out << "Blocks first expression was [" << ToString(first) << "]\n";
    return result;
  }

  return result;
}

/*
   TestFnBody exists to remove some boilerplate from testing if
   the first term within a functions body 'isa' specific kind of
   ast.

   this routine is limited as it only checks a single term from
   the functions body.
*/
template <class T>
inline auto TestFnBodyIsA(const pink::Ast::Pointer expr,
                          const pink::Location &loc, std::ostream &out)
    -> bool {
  auto *fun = llvm::dyn_cast<pink::Function>(expr);

  bool result = (fun != nullptr);
  if (!result) {
    out << "Expression was not a Function, was: " << ToString(expr) << "\n";
    return result;
  }

  result &= loc == fun->GetLocation();
  if (!result) {
    out << "Expected location: " << loc
        << " Function had location: " << fun->GetLocation() << "\n";
    return result;
  }

  auto *body = llvm::dyn_cast<pink::Block>(fun->GetBody());
  result     &= (body != nullptr);
  if (!result) {
    out << "Expected function body to be a block, instead was: "
        << ToString(fun->GetBody()) << "\n";
    return result;
  }

  result &= body->begin() != body->end();
  if (!result) {
    out << "Function body is empty: " << ToString(fun) << "\n";
    return result;
  }

  auto &body_expr = *(body->begin());
  result          &= (body_expr != nullptr);
  if (!result) {
    out << "Function Body Expression was nullptr: " << ToString(fun) << "\n";
    return result;
  }

  result &= (llvm::isa<T>(body_expr));
  if (!result) {
    out << "Function body Expression was: " << ToString(body_expr);
    return result;
  }

  return result;
}

/*
   TestBindTerm exists to remove some boilerplate from testing if
   a bind's term 'isa' specific kind of ast.
*/
template <class T>
inline auto TestBindTermIsA(const pink::Ast::Pointer expr,
                            const pink::Location &loc, std::ostream &out)
    -> bool {
  auto *bind   = llvm::dyn_cast<pink::Bind>(expr);
  bool  result = (bind != nullptr);
  if (!result) {
    out << "Expression was not a bind expression [" << ToString(expr) << "]\n";
    return result;
  }

  result &= (loc == expr->GetLocation());
  if (!result) {
    out << "Expected location: " << loc
        << " Bind has location: " << expr->GetLocation() << "\n";
    return result;
  }

  result &= (llvm::isa<T>(bind->GetAffix()));
  if (!result) {
    out << "Bind affix Expression was [" << ToString(bind->GetAffix()) << "]\n";
    return result;
  }

  return result;
}

template <class T>
inline auto
TestGetypeHoldsType(pink::Outcome<pink::Type::Pointer, pink::Error> &getype,
                    std::ostream &out, pink::Environment &env) -> bool {
  bool result = (getype);
  if (!result) {
    env.PrintErrorWithSourceText(out, getype.GetSecond());
    return result;
  }

  result = (llvm::isa<T>(getype.GetFirst()));
  if (!result) {
    out << "Actual Type: " << ToString(getype.GetFirst()) << "\n";
    return result;
  }

  return result;
}
