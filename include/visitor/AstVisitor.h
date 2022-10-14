#pragma once

#include "ast/Application.h"
#include "ast/Array.h"
#include "ast/Assignment.h"
#include "ast/Ast.h"
#include "ast/Bind.h"
#include "ast/Binop.h"
#include "ast/Block.h"
#include "ast/Bool.h"
#include "ast/Conditional.h"
#include "ast/Dot.h"
#include "ast/Function.h"
#include "ast/Int.h"
#include "ast/Nil.h"
#include "ast/Tuple.h"
#include "ast/Unop.h"
#include "ast/Variable.h"
#include "ast/While.h"

namespace pink {

/**
 * @brief Allows the visitor class to "return" a value, which can be
 * polymorphic per visitor class.
 *
 * @tparam Visitor the visitor inheriting from AstVisitorResult
 * @tparam Ptr the pointer type of this visitor
 * @tparam ResultType the result type of this visitor
 */
template <class Visitor, class Ptr, class ResultType> class AstVisitorResult {
public:
  /**
   * @brief Calls Accept just like in the normal visitor pattern,
   *  except it can return a result value with templated type.
   *  thus, different visitors can compute different result types.
   *
   * @param ptr the pointer to call Accept on
   * @return ResultType the result of the call to Accept
   */
  inline auto Compute(Ptr ptr) const -> ResultType {
    Visitor visitor;
    ptr->Accept(&visitor);
    return visitor.result;
  }

  /**
   * @brief Mimics a polymorphic return type for "Accept"
   *
   * @param result the result value to return.
   */
  inline void Return(ResultType result) const { this->result = result; }

private:
  mutable ResultType result;
};

/**
 * @brief An AstVisitor implements this interface
 *
 * \note the visitor pattern is useful in the sense that
 * the entire algorithm can reside in a single source file,
 * for a given action we want to take on a given AST. And it
 * is useful in the sense that we can write a new algorithm for
 * AST's without modifying any AST classes directly. (beyond
 * setting up the visitor pattern itself.) And it is nice in that
 * we don't have to keep going back and modifying the AST file itself,
 * so if we have a lot of algorithms to run on AST's it is nice to not
 * have the AST files themselves become massive. (to me this is the nicest
 * reason) However, it is more expensive to implement an algorithm this way.
 * compared to a virtual method in the base class, the virtual method
 * performs two regular function calls in addition to the virtual
 * Accept method, when compared to the single virtual call it takes per
 * recursive call. so it's not just inexpensive when we call the
 * Typecheck or ToString method initially, since those algorithms often
 * call that same method again to implement itself, this cost is paid every
 * time a child node is processed. So while it is only a difference between
 * 1 virtual call (Typecheck/ToString/...) for the 'regular' virtual
 * member function implementation
 * and 1 virtual call (Accept) plus two function calls (Visit, Compute)
 * per node of the tree needed to process.
 * (theoretically Visit and Compute could be inlined, But I haven't been testing
 * optimized programs yet. and Compute is static, can the compiler inline static
 * functions? idk.)
 * additionally, the abstract class must be friends with any derived visitors
 * which need to do work with the abstract classes members, which like,
 * if it's an algorithm doing something with the tree it's likely to need
 * the information within the members. Which means we don't get out of
 * modifying the abstract class every time we want to add a new algorithm.
 * or make our members public, or we could write const getters for all members.
 *
 * \note we cannot implement any form of tree mutation via a visitor
 * unless we allow the visitor to mutate the members of any given node
 * within the tree. This means A) friend classes or
 * B) non const setter/getters. either way we are breaking the concept
 * of a private data member, just to allow another class to implement
 * behavior over the tree.
 */
class AstVisitor {
public:
  virtual void Visit(const Application *application) const = 0;
  virtual void Visit(const Array *array) const = 0;
  virtual void Visit(const Assignment *assignment) const = 0;
  virtual void Visit(const Bind *bind) const = 0;
  virtual void Visit(const Binop *binop) const = 0;
  virtual void Visit(const Block *block) const = 0;
  virtual void Visit(const Bool *boolean) const = 0;
  virtual void Visit(const Conditional *conditional) const = 0;
  virtual void Visit(const Dot *dot) const = 0;
  virtual void Visit(const Function *function) const = 0;
  virtual void Visit(const Int *integer) const = 0;
  virtual void Visit(const Nil *nil) const = 0;
  virtual void Visit(const Tuple *tuple) const = 0;
  virtual void Visit(const Unop *unop) const = 0;
  virtual void Visit(const Variable *variable) const = 0;
  virtual void Visit(const While *loop) const = 0;

  AstVisitor() = default;
  AstVisitor(const AstVisitor &other) = default;
  AstVisitor(AstVisitor &&other) = default;
  auto operator=(const AstVisitor &other) -> AstVisitor & = default;
  auto operator=(AstVisitor &&other) -> AstVisitor & = default;
  virtual ~AstVisitor() = default;
};
} // namespace pink
