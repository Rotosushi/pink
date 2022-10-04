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
bool TestFnBodyIsA(pink::Ast* expr, pink::Location& loc)
{
   pink::Function*   fn = nullptr;
   pink::Block*    body = nullptr;
   pink::Ast* body_expr = nullptr;

   bool result = ((fn = llvm::dyn_cast<pink::Function>(expr)) != nullptr)
              && (loc == fn->GetLoc())
              && ((body = llvm::dyn_cast<pink::Block>(fn->body.get())) != nullptr)
              && (body->begin() != body->end())
              && ((body_expr = body->begin()->get()) != nullptr)
              && (llvm::isa<T>(body_expr));
   return result;
}

/*
   TestBindTerm exists to remove some boilerplate from testing if
   a bind's term 'isa' specific kind of ast.
*/
template <class T>
bool TestBindTermIsA(pink::Ast* expr, pink::Location& loc)
{
   pink::Bind* bind = nullptr;

   bool result = ((bind = llvm::dyn_cast<pink::Bind>(expr)) != nullptr) &&
                 (loc == expr->GetLoc()) &&
                 (llvm::isa<T>(bind->affix.get()));
   return result;
}

template <class T>
bool TestGetypeIsA(pink::Outcome<pink::Type*, pink::Error>& getype)
{
    bool result = (getype) && (llvm::isa<T>(getype.GetFirst()));
    return result;
}

pink::Ast* GetAstOrNull(pink::Outcome<std::unique_ptr<pink::Ast>, pink::Error>& outcome);


