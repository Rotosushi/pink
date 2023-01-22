#pragma once
#include <ostream>

#include "aux/Environment.h"

#include "ast/Ast.h"
#include "ast/Bind.h"
#include "ast/Block.h"
#include "ast/Conditional.h"
#include "ast/Function.h"
#include "ast/While.h"

inline auto
GetAstOrNull(std::ostream &out,
             pink::Outcome<std::unique_ptr<pink::Ast>, pink::Error> &outcome,
             pink::Environment &env) -> pink::Ast * {
  if (outcome) {
    return outcome.GetFirst().get();
  }

  env.PrintErrorWithSourceText(out, outcome.GetSecond());
  return nullptr;
}

template <class T>
inline auto TestBlocksFirstExpr(pink::Ast *expr, std::ostream &out) -> bool {
  auto *block = llvm::dyn_cast<pink::Block>(expr);

  bool result = (block == nullptr);
  if (!result) {
    out << "Expression was not a block [" << expr->ToString() << "]\n";
    return result;
  }

  result &= (block->begin() != block->end());
  if (!result) {
    out << "Block is empty [" << expr->ToString() << "]\n";
    return result;
  }

  auto *first = block->begin()->get();
  result &= (first != nullptr);
  if (!result) {
    out << "Blocks first expression was empty.\n";
    return result;
  }

  result &= (llvm::isa<T>(first));
  if (!result) {
    out << "Blocks first expression was [" << first->ToString() << "]\n";
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
    out << "Expression was not a bind expression [" << expr->ToString()
        << "]\n";
    return result;
  }

  result &= (loc == expr->GetLoc());
  if (!result) {
    out << "Expected location: " << loc
        << " Bind has location: " << expr->GetLoc() << "\n";
    return result;
  }

  result &= (llvm::isa<T>(bind->affix.get()));
  if (!result) {
    out << "Bind affix Expression was [" << bind->affix->ToString() << "]\n";
    return result;
  }

  return result;
}

template <class Test, class First, class Second>
inline auto TestConditional(pink::Ast *expr, pink::Location &loc,
                            std::ostream &out) -> bool {
  assert(expr != nullptr);
  auto *conditional = llvm::dyn_cast<pink::Conditional>(expr);

  bool result = (conditional == nullptr);
  if (!result) {
    out << "Expression was not a Conditional [" << expr->ToString() << "]\n";
    return result;
  }

  result &= (loc == expr->GetLoc());
  if (!result) {
    out << "Expected location: " << loc
        << " Actual Location: " << expr->GetLoc() << "\n";
    return result;
  }

  result &= (llvm::isa<Test>(conditional->test.get()));
  if (!result) {
    out << "Conditional test expression was [" << conditional->test->ToString()
        << "]\n";
    return result;
  }

  result &= (llvm::isa<First>(conditional->first.get()));
  if (!result) {
    out << "Conditional first expression was ["
        << conditional->first->ToString() << "]\n";
    return result;
  }

  result &= (llvm::isa<Second>(conditional->second.get()));
  if (!result) {
    out << "Conditional second expression was ["
        << conditional->second->ToString() << "]\n";
    return result;
  }

  return result;
}

template <class Test, class Body>
inline auto TestWhile(pink::Ast *expr, pink::Location &loc, std::ostream &out)
    -> bool {
  assert(expr != nullptr);
  auto *loop = llvm::dyn_cast<pink::While>(expr);

  bool result = (loop == nullptr);
  if (!result) {
    out << "Expression was not a while [" << expr->ToString() << "]\n";
    return result;
  }

  result = (expr->GetLoc() == loc);
  if (!result) {
    out << "Expected Location: " << loc
        << " Actual Location: " << expr->GetLoc() << "\n";
    return result;
  }

  result = (llvm::isa<Test>(loop->test.get()));
  if (!result) {
    out << "While test expression was [" << loop->test->ToString() << "]\n";
    return result;
  }

  result = (llvm::isa<Body>(loop->body.get()));
  if (!result) {
    out << "While body expression was [" << loop->body->ToString() << "]\n";
    return result;
  }

  return result;
}

template <class T>
inline auto
TestGetypeHoldsType(pink::Outcome<pink::Type *, pink::Error> &getype,
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
