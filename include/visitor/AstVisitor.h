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
   *  except it can return a result value
   *
   * @param ptr the pointer to call Accept on
   * @return ResultType the result of the call to Accept
   */
  inline auto Compute(Ptr ptr) -> ResultType {
    ptr->Accept(this);
    return this->result;
  }

  /**
   * @brief Mimics a polymorphic return type for "Accept"
   *
   * @param result the result value to return.
   */
  inline void Return(ResultType result) { this->result = result; }

private:
  ResultType result;
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
 */
class AstVisitor {
public:
  virtual void Visit(Application *application) = 0;
  virtual void Visit(Array *array) = 0;
  virtual void Visit(Assignment *assignment) = 0;
  virtual void Visit(Bind *bind) = 0;
  virtual void Visit(Binop *binop) = 0;
  virtual void Visit(Block *block) = 0;
  virtual void Visit(Bool *boolean) = 0;
  virtual void Visit(Conditional *conditional) = 0;
  virtual void Visit(Dot *dot) = 0;
  virtual void Visit(Function *function) = 0;
  virtual void Visit(Int *integer) = 0;
  virtual void Visit(Nil *nil) = 0;
  virtual void Visit(Tuple *tuple) = 0;
  virtual void Visit(Unop *unop) = 0;
  virtual void Visit(Variable *variable) = 0;
  virtual void Visit(While *loop) = 0;

  AstVisitor() = default;
  AstVisitor(const AstVisitor &other) = default;
  AstVisitor(AstVisitor &&other) = default;
  auto operator=(const AstVisitor &other) -> AstVisitor & = default;
  auto operator=(AstVisitor &&other) -> AstVisitor & = default;
  virtual ~AstVisitor() = default;
};
} // namespace pink
